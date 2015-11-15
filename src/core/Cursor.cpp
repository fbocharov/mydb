#include <backend/PageManager.h>

#include "Common.h"
#include "Cursor.h"

ICursor::ICursor(
		PageManager & pageManager, PageID startPageID,
		std::vector<ColumnDescriptor> const & descriptors)
	: m_descriptors(descriptors)
	, m_pageManager(pageManager)
	, m_currentPage(pageManager, startPageID, descriptors)
	, m_currentRecordNumber(0)
{}

Record ICursor::Get() {
	return m_currentPage.GetRecord(m_currentRecordNumber);
}

bool ICursor::Next() {
	size_t const recordCount = m_currentPage.GetRecordCount();
	do {
		if (recordCount == m_currentRecordNumber + 1) {
			PageID const nextPageID = m_currentPage.GetNextPageID();
			if (INVALID_PAGE_ID == nextPageID)
				return false;
			m_currentPage = DataPage(m_pageManager, nextPageID, m_descriptors);
			m_currentRecordNumber = 0;
		} else
			++m_currentRecordNumber;
	} while (!AcceptCurrentRecord());

	return true;
}
