#include <cassert>

#include <common/Condition.h>
#include <core/DataPage.h>

#include "BPlusTreeIndexCursor.h"
#include "LeafNode.h"

BPlusTreeIndexCursor::BPlusTreeIndexCursor(PageManager & manager, PageID startPageID, size_t keySize,
		ColumnDescriptors const & descriptors, Condition const & indexCondition)
	: Cursor(descriptors)
	, m_pageManager(manager)
	, m_indexKeySize(keySize)
{
	m_iterator = std::make_unique<LeafNode::EntryIterator>(
		LeafNode(m_pageManager, startPageID, m_indexKeySize).GetEntryIterator());
	// moving iterator to first entry
	while (!indexCondition.Satisfies(m_iterator->GetKey()))
		++(*m_iterator);
}

bool BPlusTreeIndexCursor::HasNext() const {
	return m_iterator->IsValid() ||
		m_iterator->GetNextLeafPageID() != INVALID_PAGE_ID;
}

char const * BPlusTreeIndexCursor::GetCurrentRecord() const {
	PageID pageID = m_iterator->GetRecordPageID();
	std::uint32_t recordNumber = m_iterator->GetRecordNumber();
	// TODO: this ctor call must be optimized later.
	return DataPage(m_pageManager, pageID, m_descriptors).GetRawRecord(recordNumber);
}

void BPlusTreeIndexCursor::GoToNextRecord() {
	if (m_iterator->IsValid())
		++(*m_iterator);
	else {
		PageID nextLeafPageID = m_iterator->GetNextLeafPageID();
		m_iterator = std::make_unique<LeafNode::EntryIterator>(
			LeafNode(m_pageManager, nextLeafPageID, m_indexKeySize).GetEntryIterator());
	}
}
