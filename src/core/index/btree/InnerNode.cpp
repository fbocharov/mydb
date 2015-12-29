#include <cassert>

#include <utils/Utils.h>

#include "InnerNode.h"

InnerNode::KeyIterator::KeyIterator(PageManager & manager, PageID pageID, uint32_t keySize, size_t keyCount)
	: m_pageManager(manager)
	, m_pageID(pageID)
	, m_keySize(keySize)
	, m_currentKeyIndex(0)
	, m_keyCount(keyCount)
{}

InnerNode::KeyIterator & InnerNode::KeyIterator::operator++ () {
	++m_currentKeyIndex;
	return *this;
}

Value InnerNode::KeyIterator::operator* () const {
	char const * bytes = m_pageManager.GetPage(m_pageID).lock()->GetData();
	size_t offset = m_keyCount * (sizeof(PageID) + m_keySize) + sizeof(PageID);
	bytes += offset;
	// NOTE: we don't need value type when using btree (?)
	// so we can leave it as UNKNOWN (is it a crunch?)
	return Value(ValueType::UNKNOWN, std::string(bytes, m_keySize));
}

bool InnerNode::KeyIterator::IsValid() const {
	return m_currentKeyIndex < m_keyCount;
}

PageID InnerNode::KeyIterator::GetPrevPageID() const {
	size_t offset = m_keyCount * (sizeof(PageID) + m_keySize);
	return GetIDByOffset(offset);
}

PageID InnerNode::KeyIterator::GetNextPageID() const {
	size_t offset = (m_keyCount + 1) * (sizeof(PageID) + m_keySize);
	return GetIDByOffset(offset);
}

PageID InnerNode::KeyIterator::GetIDByOffset(size_t offset) const {
	char const * bytes = m_pageManager.GetPage(m_pageID).lock()->GetData();
	bytes += offset;
	PageID id;
	BytesToNumber(bytes, id);
	return id;
}


InnerNode::InnerNode(PageManager & manager, PageID pageID, size_t keySize)
	: Node(manager, pageID, NodeType::INNER_NODE, keySize)
{}

InnerNode::KeyIterator InnerNode::GetKeyIterator() const {
	return KeyIterator(m_pageManager, m_pageID, m_keySize, m_keyCount);
}
