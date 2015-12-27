#include <cassert>

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

Value FullScanCursor::Get(std::string const & column) const {
	char const * record = m_currentPage->GetRawRecord(m_currentRecordNumber);
	assert(!*record); // checking for delete bit

	++record; // skip delete bit
	for (auto const & descriptor: m_descriptors) {
		if (descriptor.name == column)
			return Value{descriptor.type, std::string(record, descriptor.size)};
		record += descriptor.size;
	}

	throw std::runtime_error("Record doesn't contain field \"" + column + "\".");
}

Values FullScanCursor::GetAll() const {
	char const * record = m_currentPage->GetRawRecord(m_currentRecordNumber);
	Values values;
	++record; // skip delete bit
	for (auto const & descriptor: m_descriptors) {
		values.emplace_back(descriptor.type, std::string(record, descriptor.size));
		record += descriptor.size;
	}

	return values;
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
		}
	} while (!SatisfiesAll() && HasNext());

	return SatisfiesAll();
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

bool FullScanCursor::SatisfiesAll() const {
	// NOTE: here we use fact that all conditions are connected with AND.
	// If you wanna add OR you should also change logic here.
	char const * record = m_currentPage->GetRawRecord(m_currentRecordNumber);
	if (*record)
		return false;
	for (auto const & condition: m_conditions) {
		Value v = Get(condition.GetColumn());
		if (!condition.Satisfies(v))
			return false;
	}

	return true;
}

