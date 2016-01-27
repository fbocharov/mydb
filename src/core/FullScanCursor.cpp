#include <backend/PageManager.h>

#include "FullScanCursor.h"

FullScanCursor::FullScanCursor(PageManager & pageManager, PageID startPageID, ColumnDescriptors const & descriptors)
	: CursorImpl(descriptors)
	, m_pageManager(pageManager)
	, m_currentPage(std::make_unique<DataPage>(pageManager, startPageID, descriptors))
	, m_currentRecordNumber(-1)
	, m_firstPageID(startPageID)
{}

void FullScanCursor::GoToNextRecord() {
	if (m_currentPage->GetRecordCount() == m_currentRecordNumber) {
		PageID const nextPageID = m_currentPage->GetNextPageID();
		if (nextPageID == INVALID_PAGE_ID)
			return;
		m_currentPage = std::make_unique<DataPage>(m_pageManager, nextPageID, m_descriptors);
		m_currentRecordNumber = 0;
	} else
		++m_currentRecordNumber;
}

bool FullScanCursor::Delete() {
	return m_currentPage->DeleteRecord(m_currentRecordNumber);
}

void FullScanCursor::MoveToBegin()
{
	m_currentPage = std::make_unique<DataPage>(m_pageManager, m_firstPageID, m_descriptors);
	m_currentRecordNumber = -1;
}

char const * FullScanCursor::GetCurrentRecord() const {
	return m_currentPage->GetRawRecord(m_currentRecordNumber);
}

bool FullScanCursor::HasNext() const {
	return m_currentRecordNumber + 1 < m_currentPage->GetRecordCount() ||
		m_currentPage->GetNextPageID() != INVALID_PAGE_ID;
}
