#include <cassert>

#include <backend/PageManager.h>

#include "FullScanCursor.h"

FullScanCursor::FullScanCursor(PageManager & pageManager, PageID startPageID, ColumnDescriptors const & descriptors,
		Conditions const & conditions)
	: Cursor(descriptors)
	, m_pageManager(pageManager)
	, m_currentPage(std::make_unique<DataPage>(pageManager, startPageID, descriptors))
	, m_currentRecordNumber(0)
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

bool FullScanCursor::Update(std::map<std::string, Value> const & colVals) {
	return m_currentPage->UpdateRecord(m_currentRecordNumber, colVals);
}

bool FullScanCursor::Delete() {
	return m_currentPage->DeleteRecord(m_currentRecordNumber);
}

char const * FullScanCursor::GetCurrentRecord() const {
	return m_currentPage->GetRawRecord(m_currentRecordNumber);
}

bool FullScanCursor::HasNext() const {
	return m_currentRecordNumber < m_currentPage->GetRecordCount() ||
		m_currentPage->GetNextPageID() != INVALID_PAGE_ID;
}
