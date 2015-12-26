#include <backend/PageManager.h>

#include "FullScanCursor.h"

FullScanCursor::FullScanCursor(PageManager & pageManager, PageID startPageID, ColumnDescriptors const & descriptors,
		Conditions const & conditions)
	: m_descriptors(descriptors)
	, m_pageManager(pageManager)
	, m_currentPage(std::make_unique<DataPage>(pageManager, startPageID, descriptors))
	, m_currentRecordNumber(-1)
	, m_conditions(conditions)
{}

Record const & FullScanCursor::Get() const {
	return m_currentRecord;
}

bool FullScanCursor::Next() {
	if (!HasNext())
		return false;

	do {
		if (m_currentPage->GetRecordCount() == m_currentRecordNumber + 1) {
			PageID const nextPageID = m_currentPage->GetNextPageID();
			m_currentPage = std::make_unique<DataPage>(m_pageManager, nextPageID, m_descriptors);
			m_currentRecordNumber = 0;
		} else {
			++m_currentRecordNumber;
			m_currentRecord = m_currentPage->GetRecord(m_currentRecordNumber);
		}
	} while ((!SatisfiesAll(m_currentRecord) || m_currentRecord.IsDeleted()) && HasNext());

	return SatisfiesAll(m_currentRecord) && !m_currentRecord.IsDeleted();
}

bool FullScanCursor::Update(std::map<std::string, Value> const & colVals) {
	return m_currentPage->UpdateRecord(m_currentRecordNumber, colVals);
}

bool FullScanCursor::Delete() {
	return m_currentPage->DeleteRecord(m_currentRecordNumber);
}

bool FullScanCursor::HasNext() const {
	return m_currentRecordNumber + 1 < m_currentPage->GetRecordCount() ||
		m_currentPage->GetNextPageID() != INVALID_PAGE_ID;
}

bool FullScanCursor::SatisfiesAll(Record const & record) const {
	// NOTE: here we use fact that all conditions are connected with AND.
	// If you wanna add OR you should also change logic here.
	for (auto condition: m_conditions) {
		bool ok = condition.Satisfies(record);
		if (!ok)
			return false;
	}

	return true;
}
