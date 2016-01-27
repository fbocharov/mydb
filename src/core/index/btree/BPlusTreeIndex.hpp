#ifndef BPlusTreeIndex_hpp
#define BPlusTreeIndex_hpp

#include <cassert>

#include <backend/PageManager.h>
#include <core/index/Index.hpp>

#include "Node.h"
#include "InnerNode.h"
#include "LeafNode.h"
#include "BPlusTreeIndexCursor.h"
#include <core/QueryExecutor.h>

inline std::unique_ptr<Node> CreateNode(PageManager & manager, PageID pageID, size_t keySize) {
	auto page = manager.GetPage(pageID).lock();
	char const * bytes = page->GetData();
	NodeType type;
	BytesToNumber(bytes, type);
	switch (type) {
		case NodeType::INNER_NODE:
			return std::make_unique<InnerNode>(manager, pageID, keySize);
		case NodeType::LEAF_NODE:
			return std::make_unique<LeafNode>(manager, pageID, keySize);
		default:
			throw std::runtime_error("Index page contains incorrect node type.");
	}
	return nullptr;
}


template<typename KeyT>
class BPlusTreeIndex : public IIndex {
public:
	BPlusTreeIndex(std::string const & name, bool isUnique,
		std::shared_ptr<PageManager> manager, PageID rootPageID);

	virtual bool Insert(Value key, PageID pageID, std::uint32_t recordNumber) override;
	virtual size_t RemoveSatisfying(Condition const & condition) override;
	virtual std::unique_ptr<ICursor> GetCursor(Condition const & indexCondition) override;

private:
	PageID FindLeaf(PageID pageID, Condition const & condition) const;

private:
	std::shared_ptr<PageManager> m_pageManager;
	PageID m_rootPageID;
};


template<typename KeyT>
BPlusTreeIndex<KeyT>::BPlusTreeIndex(std::string const & name, bool isUnique,
		std::shared_ptr<PageManager> manager, PageID rootPageID)
	: IIndex(name, isUnique)
	, m_pageManager(manager)
	, m_rootPageID(rootPageID)
{}

template<typename KeyT>
bool BPlusTreeIndex<KeyT>::Insert(Value key, PageID pageID, std::uint32_t recordNumber) {
	assert(false);
	return false;
}

template<typename KeyT>
size_t BPlusTreeIndex<KeyT>::RemoveSatisfying(Condition const & condition) {
	assert(false);
	return 0;
}

template<typename KeyT>
std::unique_ptr<ICursor> BPlusTreeIndex<KeyT>::GetCursor(Condition const & indexCondition) {
	auto leafPageID = FindLeaf(m_rootPageID, indexCondition);
	return std::make_unique<BPlusTreeIndexCursor>(m_pageManager, leafPageID, indexCondition);
}

template<typename KeyT>
PageID BPlusTreeIndex<KeyT>::FindLeaf(PageID pageID, Condition const & condition) const {
	auto node = CreateNode(m_pageManager, pageID, sizeof(KeyT));
	if (node->IsLeaf())
		return pageID;

	auto innerNode = static_cast<InnerNode const &>(*node.get());
	auto it = innerNode.GetKeyIterator();
	for ( ; it.IsValid(); ++it)
		if (!condition.Satisfies(*it))
			break;

	if (it.IsValid()) // stopped in the middle of key list -> need take prev
		return FindLeaf(it.GetPrevPageID(), condition);
	// otherwise need to take last
	return FindLeaf(it.GetNextPageID(), condition);
}

#endif // BPlusTreeIndex_hpp
