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
	, m_columnDescriptors(descrpitors)
	, m_recordLength(0)
{
	auto page = GetNativePage();
	m_nextPageID = page->GetNextPageID();
	m_prevPageID = page->GetPrevPageID();

	m_recordLength = 1; // TODO: replace it with sizeof record flags.
	for (auto const & descriptor: descrpitors) {
		m_columnOffsets[descriptor.name] = m_recordLength;
		m_recordLength += descriptor.size;
	}

	ReadHeader(page->GetData());
}

DataPage::~DataPage() {
	if (m_pageManager.PageInCache(m_id))
		m_pageManager.GetPage(m_id).lock()->Unpin();
}

bool DataPage::AppendRecord(std::map<std::string, Value> const & colVals) {
	if (!HasFreeSpace())
		return false;

	auto page = GetNativePage(true);
	try {
		size_t number = m_recordCount;
		++m_recordCount;
		UpdateRecord(number, colVals);
	} catch (...) {
		--m_recordCount;
		throw;
	}

	m_freeSpaceOffset += m_recordLength;
	WriteHeader(page->GetData());

	return true;
}

bool DataPage::UpdateRecord(size_t number, std::map<std::string, Value> const & colVals) {
	auto page = GetNativePage(true);
	size_t const offset = CalculateRecordOffset(number);

	char * data = page->GetData() + offset;
	if (*data) // delete bit is set
		return false;

	for (auto const & colVal: colVals) {
		auto const & descriptor = GetDescriptorByName(m_columnDescriptors, colVal.first);
		if (!colVal.second.IsEmpty() && descriptor.type != colVal.second.GetType()) {
			std::string field(descriptor.name);
			throw std::runtime_error("Invalid value for field \"" + field + "\".");
		}
	}
	// Values should be inserted after check that all of them are valid.
	for (auto const & colVal: colVals) {
		size_t const colOffset = m_columnOffsets[colVal.first];
		memcpy(data + colOffset, colVal.second.GetBytes(), colVal.second.GetSize());
	}

	return true;
}

bool DataPage::DeleteRecord(size_t number) {
	size_t const offset = CalculateRecordOffset(number);

	char * recordData = GetNativePage(true)->GetData() + offset;
	*recordData = 1; // Raising up delete bit. TODO: make constant for delete bit.
	return true;
}

char const * DataPage::GetRawRecord(size_t number) const {
	size_t const offset = CalculateRecordOffset(number);
	auto page = GetNativePage();

	return page->GetData() + offset;
}

size_t DataPage::GetRecordCount() const {
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

size_t DataPage::CalculateRecordOffset(size_t number) const {
	std::string const error = "Trying to access record with incorrect number:" + std::to_string(number) + ".";

	if (m_recordCount <= number)
		throw std::runtime_error(error);

	size_t const offset = HEADER_SIZE + m_recordLength * number;
	if (offset + m_recordLength > Page::PAGE_DATA_SIZE)
		throw std::runtime_error(error);

	return offset;
}

void DataPage::ReadHeader(char const * data) {
	// NOTE: this header should always be valid for every page. If page is empty,
	// it must contain zeros but not memory trash. If not empty it must contain
	// valid info.
	BytesToNumber(data, m_recordCount);
	data += sizeof(m_recordCount);
	BytesToNumber(data, m_freeSpaceOffset);
}

void DataPage::WriteHeader(char * data) {
	NumberToBytes(m_recordCount, data);
	data += sizeof(m_recordCount);
	NumberToBytes(m_freeSpaceOffset, data);
}

std::shared_ptr<Page> DataPage::GetNativePage(bool needDirty) const {
	auto page = m_pageManager.GetPage(m_id).lock();
	page->Pin();
	if (needDirty)
		page->SetDirty();
	return page;
}
