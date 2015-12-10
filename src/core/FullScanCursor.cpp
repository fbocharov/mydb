#include <backend/PageManager.h>

#include "FullScanCursor.h"

FullScanCursor::FullScanCursor(PageManager & pageManager, PageID startPageID, ColumnDescriptors const & descriptors)
	: m_descriptors(descriptors)
	, m_pageManager(pageManager)
	, m_currentPage(std::make_unique<DataPage>(pageManager, startPageID, descriptors))
	, m_currentRecordNumber(-1)
{}

Record FullScanCursor::Get() {
	return m_currentPage->GetRecord(m_currentRecordNumber);
}

bool FullScanCursor::Next() {
	if (!HasNext())
		return false;

	if (m_currentPage->GetRecordCount() == m_currentRecordNumber + 1) {
		PageID const nextPageID = m_currentPage->GetNextPageID();
		m_currentPage = std::make_unique<DataPage>(m_pageManager, nextPageID, m_descriptors);
		m_currentRecordNumber = 0;
	} else
		++m_currentRecordNumber;

	return true;
}

bool FullScanCursor::HasNext() const {
	return m_currentRecordNumber + 1 < m_currentPage->GetRecordCount() ||
		m_currentPage->GetNextPageID() != INVALID_PAGE_ID;
}
