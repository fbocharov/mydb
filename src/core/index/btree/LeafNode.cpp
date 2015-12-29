#include "LeafNode.h"

LeafNode::EntryIterator::EntryIterator(PageManager & manager, PageID pageID,
		std::uint32_t entryCount, size_t keySize)
	: m_pageManager(manager)
	, m_pageID(pageID)
	, m_currentEntryIndex(0)
	, m_entryCount(entryCount)
	, m_keySize(keySize)
	, m_entrySize(keySize + sizeof(PageID) + sizeof(std::uint32_t))
{}

LeafNode::EntryIterator & LeafNode::EntryIterator::operator++ () {
	++m_currentEntryIndex;
	return *this;
}

bool LeafNode::EntryIterator::IsValid() const {
	return m_currentEntryIndex < m_entryCount;
}

Value LeafNode::EntryIterator::GetKey() const {
	char const * bytes = m_pageManager.GetPage(m_pageID).lock()->GetData();
	size_t offset = m_currentEntryIndex * m_entrySize;
	bytes += offset;
	return Value(ValueType::UNKNOWN, std::string(bytes, m_keySize));
}


PageID LeafNode::EntryIterator::GetRecordPageID() const {
	size_t offset = m_currentEntryIndex * m_entrySize + m_keySize;
	return GetDataByOffset<PageID>(offset);
}

std::uint32_t LeafNode::EntryIterator::GetRecordNumber() const {
	size_t offset = m_currentEntryIndex * m_entrySize + m_keySize + sizeof(PageID);
	return GetDataByOffset<std::uint32_t>(offset);
}

PageID LeafNode::EntryIterator::GetNextLeafPageID() const {
	return m_pageManager.GetPage(m_pageID).lock()->GetNextPageID();
}


LeafNode::LeafNode(PageManager & manager, PageID pageID, size_t keySize)
	: Node(manager, pageID, NodeType::LEAF_NODE, keySize)
{}

LeafNode::EntryIterator LeafNode::GetEntryIterator() const {
	return EntryIterator(m_pageManager, m_pageID, m_keyCount, m_keySize);
}
