#ifndef BPlusTreeIndex_hpp
#define BPlusTreeIndex_hpp

#include <cassert>

#include <backend/PageManager.h>
#include <core/index/Index.h>
#include <core/ICursor.h>

#include "Node.hpp"
#include "InnerNode.hpp"
#include "LeafNode.hpp"
#include "BPlusTreeIndexCursor.h"


template<typename KeyT, size_t TreeOrder = 64>
class BPlusTreeIndex : public Index {
public:
	BPlusTreeIndex(bool isUnique, std::shared_ptr<PageManager> manager);
	BPlusTreeIndex(bool isUnique, std::shared_ptr<PageManager> manager, PageID rootPage);

	void Serialize(char * bytes) const;
	static std::unique_ptr<Index> Deserialize(char * bytes, bool unique, std::shared_ptr<PageManager> manager);

	virtual bool Insert(Value const & key, PageID pageID, std::uint32_t recordNumber) override;
	virtual std::unique_ptr<InternalCursor> GetCursor(ColumnDescriptors const & descriptors, Condition const & condition) override;

private:
	PageID FindLeaf(PageID nodeID, Condition const & condition) const;
	PageID FindLeafToInsert(PageID nodeID, KeyT key);
	void SplitChild(PageID parentID, PageID childID);

private:
	std::shared_ptr<PageManager> m_pageManager;
	PageID m_rootPageID;
};


template<typename KeyT, size_t TreeOrder>
BPlusTreeIndex<KeyT, TreeOrder>::BPlusTreeIndex(bool isUnique, std::shared_ptr<PageManager> manager)
	: Index(isUnique, IndexType::BTREE)
	, m_pageManager(manager)
{
	auto page = m_pageManager->AllocatePage().lock();
	m_rootPageID = page->GetID();
	LeafNode<KeyT>::InitNode(page->GetData());
}

template<typename KeyT, size_t TreeOrder>
BPlusTreeIndex<KeyT, TreeOrder>::BPlusTreeIndex(bool isUnique, std::shared_ptr<PageManager> manager, PageID rootPage)
	: Index(isUnique, IndexType::BTREE)
	, m_pageManager(manager)
	, m_rootPageID(rootPage)
{}

template<typename KeyT, size_t TreeOrder>
std::unique_ptr<Index> BPlusTreeIndex<KeyT, TreeOrder>::Deserialize(char * bytes, bool unique, std::shared_ptr<PageManager> manager) {
	PageID rootPage;
	BytesToNumber(bytes, rootPage);
	return std::make_unique<BPlusTreeIndex<KeyT, TreeOrder>>(unique, manager, rootPage);
}

template<typename KeyT, size_t TreeOrder>
bool BPlusTreeIndex<KeyT, TreeOrder>::Insert(Value const & key, PageID pageID, std::uint32_t recordNumber) {
	KeyT k = key.Get<KeyT>();
	PageID leafID = FindLeafToInsert(m_rootPageID, k);

	char * bytes = m_pageManager->GetPage(leafID).lock()->GetData();
	LeafNode<KeyT> leaf(bytes);
	leaf.Insert(k, pageID, recordNumber, m_isUnique);

	bytes = m_pageManager->GetPage(m_rootPageID).lock()->GetData();
	Node node(bytes);
	if (node.GetEntryCount() == TreeOrder) {
		PageID newRootPage = m_pageManager->AllocatePage().lock()->GetID();
		SplitChild(newRootPage, m_rootPageID);
		m_rootPageID = newRootPage;
	}

	return true;
}

template<typename KeyT, size_t TreeOrder>
std::unique_ptr<InternalCursor> BPlusTreeIndex<KeyT, TreeOrder>::GetCursor(ColumnDescriptors const & descriptors,
		Condition const & condition)
{
	auto leafID = FindLeaf(m_rootPageID, condition);
	auto page = m_pageManager->GetPage(leafID).lock();
	LeafNode<KeyT> leaf(page->GetData());

	size_t i = 0;
	if (condition.GetOperation() != '<') {
		for (; i < leaf.GetEntryCount(); ++i) {
			KeyT key = leaf.GetKey(i);
			if (condition.Satisfies(Value(ValueType::UNKNOWN, key)))
				break;
		}
	}

	return std::make_unique<BPlusTreeIndexCursor<KeyT>>(
		descriptors, *m_pageManager, condition, page->GetID(), --i);
}

template<typename KeyT, size_t TreeOrder>
void BPlusTreeIndex<KeyT, TreeOrder>::Serialize(char * bytes) const {
	NumberToBytes(m_rootPageID, bytes);
}

template<typename KeyT, size_t TreeOrder>
PageID BPlusTreeIndex<KeyT, TreeOrder>::FindLeaf(PageID nodeID, Condition const & condition) const {
	char * bytes = m_pageManager->GetPage(nodeID).lock()->GetData();
	if (Node(bytes).IsLeaf())
		return nodeID;

	PageID childID;
	InnerNode<KeyT> node(bytes);
	size_t i = 0;
	if ('<' != condition.GetOperation()) {
		for (; i < node.GetEntryCount(); ++i) {
			KeyT key = node.GetKey(i);
			if (key >= condition.GetValue().Get<KeyT>())
				break;
		}
	}
	childID = node.GetLEChildPage(i);

	return FindLeaf(childID, condition);
}

template<typename KeyT, size_t TreeOrder>
PageID BPlusTreeIndex<KeyT, TreeOrder>::FindLeafToInsert(PageID nodeID, KeyT key) {
	char * bytes = m_pageManager->GetPage(nodeID).lock()->GetData();
	if (Node(bytes).IsLeaf())
		return nodeID;

	InnerNode<KeyT> innerNode(bytes);
	PageID childID = innerNode.GetNextNodePage(key);

	PageID leafID = FindLeafToInsert(childID, key);

	bytes = m_pageManager->GetPage(childID).lock()->GetData();
	if (Node(bytes).GetEntryCount() == TreeOrder)
		SplitChild(nodeID, childID);

	return leafID;
}

template<typename KeyT, size_t TreeOrder>
void BPlusTreeIndex<KeyT, TreeOrder>::SplitChild(PageID parentID, PageID childID) {
	auto newNodePage = m_pageManager->AllocatePage().lock();
	auto childPage = m_pageManager->GetPage(childID).lock();

	char * childBytes = childPage->GetData();
	KeyT key;
	if (Node(childBytes).IsLeaf()) {
		key = LeafNode<KeyT>(childBytes).TransferHalf(newNodePage->GetData());

		newNodePage->SetPrevPageID(childPage->GetID());
		newNodePage->SetNextPageID(childPage->GetNextPageID());
		childPage->SetNextPageID(newNodePage->GetID());
	} else
		key = InnerNode<KeyT>(childBytes).TransferHalf(newNodePage->GetData());

	char * parentBytes = m_pageManager->GetPage(parentID).lock()->GetData();
	InnerNode<KeyT> parent(parentBytes);
	if (0 == parent.GetEntryCount())
		InnerNode<KeyT>::InitNode(parentBytes, key, childID, newNodePage->GetID());
	else
		parent.Insert(key, newNodePage->GetID());
}

#endif // BPlusTreeIndex_hpp
