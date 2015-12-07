#include <stdexcept>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cstdio>

#include <backend/PageManager.h>
#include <utils/Utils.h>

#include "DataPage.h"

DataPage::DataPage(PageManager & manager, PageID pageID, ColumnDescriptors const & descrpitors)
	: m_pageManager(manager)
	, m_id(pageID)
	, m_recordLength(0)
{
	auto page = GetNativePage();
	m_nextPageID = page->GetNextPageID();
	m_prevPageID = page->GetPrevPageID();

	m_recordLength = 1; // TODO: replace it with sizeof record flags.
	for (auto const & descriptor: descrpitors) {
		m_columnDescriptors[descriptor.name] = descriptor;
		m_columnOffsets[descriptor.name] = m_recordLength;
		m_recordLength += descriptor.size;
	}

	ReadHeader(page->GetData());
}

DataPage::~DataPage() {
	if (m_pageManager.PageInCache(m_id))
		m_pageManager.GetPage(m_id).lock()->Unpin();
}

bool DataPage::AppendRecord(std::map<std::string, std::string> const & colVals) {
	if (!HasFreeSpace())
		return false;

	auto page = GetNativePage(true);
	UpdateRecord(m_recordCount, colVals);

	++m_recordCount;
	m_freeSpaceOffset += m_recordLength;
	WriteHeader(page->GetData());

	return true;
}

void DataPage::UpdateRecord(size_t number, std::map<std::string, std::string> const & colVals) {
	auto page = GetNativePage(true);
	uint16_t const offset = CalculateRecordOffset(number);

	char * data = page->GetData() + offset;
	if (*data) // delete bit is set
		return;
	data += 1; // TODO: replace by sizeof(flags)

	for (auto const & colVal: colVals) {
		uint16_t const colOffset = m_columnOffsets[colVal.first];
		auto const & descriptor = m_columnDescriptors[colVal.first];
		if (!CheckType(descriptor, colVal.second)) {
			std::string field(descriptor.name);
			throw std::runtime_error("Invalid value for field '" + field + "'.");
		}
		::memcpy(data + colOffset, colVal.second.data(), colVal.second.length());
	}
}

void DataPage::DeleteRecord(size_t number) {
	uint16_t const offset = CalculateRecordOffset(number);

	char * recordData = GetNativePage(true)->GetData() + offset;
	*recordData = 1; // Raising up delete bit. TODO: make constant for delete bit.
}

Record DataPage::GetRecord(size_t number) {
	if (m_recordCount <= number)
		throw std::out_of_range("Trying to access record with too big number.");

	uint16_t const offset = CalculateRecordOffset(number);
	auto page = GetNativePage();

	return Record(*this, number, page->GetData() + offset, m_columnDescriptors);
}

uint16_t DataPage::GetRecordCount() const {
	return m_recordCount;
}

PageID DataPage::GetID() const {
	return m_id;
}

PageID DataPage::GetNextPageID() const {
	return m_nextPageID;
}

PageID DataPage::GetPrevPageID() const {
	return m_prevPageID;
}

bool DataPage::HasFreeSpace() const {
	return Page::PAGE_DATA_SIZE >= m_freeSpaceOffset + m_recordLength;
}

uint16_t DataPage::CalculateRecordOffset(size_t recordNumber) const {
	return HEADER_SIZE + m_recordLength * recordNumber;
}

void DataPage::ReadHeader(char const * data) {
	// NOTE: this header should always be valid for every page. If page is empty,
	// it must contain zeros but not memory trash. If not empty it must contain
	// valid info.
	BytesToNumber(data, m_recordCount);
	data += sizeof(uint16_t);
	BytesToNumber(data, m_freeSpaceOffset);
}

void DataPage::WriteHeader(char * data) {
	NumberToBytes(m_recordCount, data);
	data += sizeof(uint16_t);
	NumberToBytes(m_freeSpaceOffset, data);
}

std::shared_ptr<Page> DataPage::GetNativePage(bool needDirty) const {
	auto page = m_pageManager.GetPage(m_id).lock();
	page->Pin();
	if (needDirty)
		page->SetDirty();
	return page;
}

bool DataPage::CheckType(ColumnDescriptor const & descriptor, std::string const & value) {
	std::stringstream checker(value);
	if (descriptor.size >= value.length()) {
		switch (descriptor.type) {
			case FieldType::DOUBLE:
				double d;
				return bool(checker >> d);
			case FieldType::INT:
				int i;
				return bool(checker >> i);
			case FieldType::VARCHAR:
				return VARCHAR_MAX_LENGTH > value.length();
		}
	}

	return false;
}
