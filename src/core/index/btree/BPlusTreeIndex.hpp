#ifndef BPlusTreeIndex_hpp
#define BPlusTreeIndex_hpp

#include <cassert>

#include <backend/PageManager.h>
#include <core/index/Index.hpp>
#include <core/Cursor.h>

#include "Node.hpp"
#include "InnerNode.hpp"
#include "LeafNode.hpp"
#include "BPlusTreeIndexCursor.h"


template<typename KeyT, size_t TreeOrder = 64>
class BPlusTreeIndex : public IIndex {
public:
	BPlusTreeIndex(bool isUnique, std::shared_ptr<PageManager> manager);

	virtual bool Insert(Value const & key, PageID pageID, std::uint32_t recordNumber) override;
	virtual std::unique_ptr<SelectCursor> GetCursor(Condition const & condition) override;

private:
	PageID FindLeafToInsert(PageID nodeID, KeyT key);
	void SplitChild(PageID parentID, PageID childID);

private:
	std::shared_ptr<PageManager> m_pageManager;
	PageID m_rootPageID;
};


template<typename KeyT, size_t TreeOrder>
BPlusTreeIndex<KeyT, TreeOrder>::BPlusTreeIndex(bool isUnique, std::shared_ptr<PageManager> manager)
	: IIndex(isUnique)
	, m_pageManager(manager)
{}

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
std::unique_ptr<SelectCursor> BPlusTreeIndex<KeyT, TreeOrder>::GetCursor(Condition const & condition) {
	throw 1;
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
