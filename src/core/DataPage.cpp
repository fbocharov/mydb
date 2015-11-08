#include <stdexcept>
#include <cassert>
#include <cstring>

#include <utils/Utils.h>

#include "DataPage.h"

DataPage::DataPage(PageManager & manager,
		PageID pageID, std::vector<ColumnDescriptor> const & descrpitors)
	: m_pageManager(&manager)
	, m_currentPageID(pageID)
	, m_columnDescriptors(&descrpitors)
	, m_recordLength(0)
{
	if (!m_pageManager)
		throw std::logic_error("Trying to create DataPage with null page manager!");

	auto page = GetNativePage();
	m_nextPageID = page->GetNextPageID();
	m_prevPageID = page->GetPrevPageID();

	for (auto const & descriptor: *m_columnDescriptors)
		m_recordLength += descriptor.size;
	m_recordLength += 1; // TODO: replace it with sizeof record flags.

	ReadHeader(page->GetData());
}

DataPage::~DataPage() {
	// TODO: unpin current page if it's in page cache.
}

bool DataPage::AppendRecord(Record const & record) {
	if (!HasFreeSpace())
		return false;

	auto page = GetNativePage(true);
	assert(false && "AppendRecord not implemented yet.");

	++m_recordCount;
	m_freeSpaceOffset += m_recordLength;
	WriteHeader(page->GetData());

	return true;
}

void DataPage::DeleteRecord(size_t number) {
	uint16_t const offset = CalculateRecordOffset(number);

	char * recordData = GetNativePage(true)->GetData() + offset;
	*recordData = 1; // Raising up delete bit.
}

Record DataPage::GetRecord(size_t number) const {
	if (m_recordCount <= number)
		throw std::out_of_range("Trying to access record with too big number.");

	uint16_t const offset = CalculateRecordOffset(number);
	auto page = GetNativePage();

	return Record(*m_columnDescriptors, page->GetData() + offset);
}

uint16_t DataPage::GetRecordCount() const {
	return m_recordCount;
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
	auto page = m_pageManager->GetPage(m_currentPageID).lock();
	page->Pin();
	if (needDirty)
		page->SetDirty();
	return page;
}
