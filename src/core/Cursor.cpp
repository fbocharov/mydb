#include <backend/PageManager.h>

#include "Cursor.h"
#include "Common.h"

ICursor::ICursor(
		std::vector<ColumnDescriptor> const & descriptors,
		PageManager & pageManager, PageID startPageID)
	: m_descriptors(descriptors)
	, m_pageManager(pageManager)
	, m_currentPageID(startPageID)
	, m_currentRecordID(0)
	, m_currentRecord(m_descriptors, nullptr)
	, m_recordLength(0)
{
	for (auto const & descriptor : m_descriptors)
		m_recordLength += descriptor.size;
	m_recordLength += 1; // TODO: remove 1 and insert sizeof flags here!
}

Record & ICursor::Get() {
	auto pagePtr = m_pageManager.GetPage(m_currentPageID).lock();
	// Here we assume that m_currentRecordID will change after every Next call.
//	size_t const offset = pagePtr->GetSpaceOffset() + m_currentRecordID * m_recordLength;
//	char * data = pagePtr->GetData() + offset;
//	m_currentRecord = Record(m_descriptors, data);

	return m_currentRecord;
}

bool ICursor::Next() {
	return false;
}
