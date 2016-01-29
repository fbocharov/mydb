#ifndef BPlusTreeIndex_hpp
#define BPlusTreeIndex_hpp

#include <cassert>
#include <iostream>

#include <backend/PageManager.h>
#include <core/index/Index.h>
#include <core/ICursor.h>

#include "Node.hpp"
#include "InnerNode.hpp"
#include "LeafNode.hpp"
#include "BPlusTreeIndexCursor.h"


template<typename KeyT, size_t TreeOrder = 128>
class BPlusTreeIndex : public Index {
public:
	BPlusTreeIndex(bool isUnique, std::shared_ptr<PageManager> manager);
	BPlusTreeIndex(bool isUnique, std::shared_ptr<PageManager> manager, PageID rootPage);

	virtual char * Serialize(char * bytes) const override;
	static std::shared_ptr<Index> Deserialize(char const ** bytes, bool unique,
		std::shared_ptr<PageManager> & manager);

	virtual bool Insert(Value const & key, PageID pageID, std::uint32_t recordNumber) override;
	virtual std::unique_ptr<InternalCursor> GetCursor(ColumnDescriptors const & descriptors,
		Condition const & from, Condition const & to) override;

private:
	PageID FindLeaf(PageID nodeID, Condition const & condition) const;
	void DoInsert(PageID nodeID, KeyT key, PageID recordPage, uint32_t recordNumber);
	void SplitChild(PageID parentID, PageID leftID);

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
	page->SetNextPageID(INVALID_PAGE_ID);
	page->SetDirty();
}

template<typename KeyT, size_t TreeOrder>
BPlusTreeIndex<KeyT, TreeOrder>::BPlusTreeIndex(bool isUnique, std::shared_ptr<PageManager> manager, PageID rootPage)
	: Index(isUnique, IndexType::BTREE)
	, m_pageManager(manager)
	, m_rootPageID(rootPage)
{}

template<typename KeyT, size_t TreeOrder>
char * BPlusTreeIndex<KeyT, TreeOrder>::Serialize(char * bytes) const {
	NumberToBytes(m_rootPageID, bytes);
	return bytes + sizeof(m_rootPageID);
}

template<typename KeyT, size_t TreeOrder>
std::shared_ptr<Index> BPlusTreeIndex<KeyT, TreeOrder>::Deserialize(char const ** bytes, bool isUnique,
		std::shared_ptr<PageManager> & manager)
{
	PageID rootPage;
	BytesToNumber(*bytes, rootPage);
	*bytes += sizeof(rootPage);
	return std::make_shared<BPlusTreeIndex<KeyT, TreeOrder>>(isUnique, manager, rootPage);
}

template<typename KeyT, size_t TreeOrder>
bool BPlusTreeIndex<KeyT, TreeOrder>::Insert(Value const & key, PageID pageID, std::uint32_t recordNumber) {
	if (recordNumber == 16777219)
		std::cerr << "Got it! " << key.Get<KeyT>() << std::endl;


	DoInsert(m_rootPageID, key.Get<KeyT>(), pageID, recordNumber);

	auto rootPage = m_pageManager->GetPage(m_rootPageID).lock();
	Node root(rootPage->GetData());
	if (root.GetEntryCount() == TreeOrder) {
		auto newRootPage = m_pageManager->AllocatePage().lock();
		SplitChild(newRootPage->GetID(), m_rootPageID);
		m_rootPageID = newRootPage->GetID();
		newRootPage->SetDirty();
	}
	rootPage->SetDirty();

	return true;
}

template<typename KeyT, size_t TreeOrder>
std::unique_ptr<InternalCursor> BPlusTreeIndex<KeyT, TreeOrder>::GetCursor(ColumnDescriptors const & descriptors,
		Condition const & from, Condition const & to)
{
	auto leafID = FindLeaf(m_rootPageID, from);
	auto page = m_pageManager->GetPage(leafID).lock();
	LeafNode<KeyT> leaf(page->GetData());

	size_t i = 0;
	if (from.GetOperation() != '<') {
		for (; i < leaf.GetEntryCount(); ++i) {
			KeyT key = leaf.GetKey(i);
			if (from.Satisfies(Value(ValueType::UNKNOWN, key)))
				break;
		}
	}

	return std::make_unique<BPlusTreeIndexCursor<KeyT>>(
		descriptors, *m_pageManager, from, to, page->GetID(), --i);
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
			if (key > condition.GetValue().Get<KeyT>())
				break;
		}
	}
	childID = node.GetLEChildPage(i);

	return FindLeaf(childID, condition);
}

template<typename KeyT, size_t TreeOrder>
void BPlusTreeIndex<KeyT, TreeOrder>::DoInsert(PageID nodeID, KeyT key, PageID recordPage, std::uint32_t recordNumber) {
	char * bytes = m_pageManager->GetPage(nodeID).lock()->GetData();
	if (Node(bytes).IsLeaf()) {
		LeafNode<KeyT> leaf(bytes);
		leaf.Insert(key, recordPage, recordNumber, m_isUnique);
		m_pageManager->GetPage(nodeID).lock()->SetDirty();
		return;
	}

	InnerNode<KeyT> innerNode(bytes);
	PageID childID = innerNode.GetNextNodePage(key);

	DoInsert(childID, key, recordPage, recordNumber);

	bytes = m_pageManager->GetPage(childID).lock()->GetData();
	if (Node(bytes).GetEntryCount() == TreeOrder)
		SplitChild(nodeID, childID);
}

template<typename KeyT, size_t TreeOrder>
void BPlusTreeIndex<KeyT, TreeOrder>::SplitChild(PageID parentID, PageID leftID) {
	auto rightPage = m_pageManager->AllocatePage().lock();
	auto leftPage = m_pageManager->GetPage(leftID).lock();

	char * leftBytes = leftPage->GetData();
	KeyT key;
	if (Node(leftBytes).IsLeaf()) {
		key = LeafNode<KeyT>(leftBytes).TransferHalf(rightPage->GetData());

		rightPage->SetPrevPageID(leftPage->GetID());
		rightPage->SetNextPageID(leftPage->GetNextPageID());
		leftPage->SetNextPageID(rightPage->GetID());
	} else
		key = InnerNode<KeyT>(leftBytes).TransferHalf(rightPage->GetData());

	auto parentPage = m_pageManager->GetPage(parentID).lock();
	char * parentBytes = parentPage->GetData();
	InnerNode<KeyT> parent(parentBytes);
	if (0 == parent.GetEntryCount())
		InnerNode<KeyT>::InitNode(parentBytes, key, leftID, rightPage->GetID());
	else
		parent.Insert(key, rightPage->GetID());

	parentPage->SetDirty();
	rightPage->SetDirty();
	leftPage->SetDirty();
}

#endif // BPlusTreeIndex_hpp
