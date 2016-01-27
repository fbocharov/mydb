#include <backend/PageManager.h>
#include <utils/Log.h>
#include <utils/Utils.h>

#include "Table.h"
#include "FullScanCursor.h"

using std::uint32_t;

Table::Table(std::shared_ptr<PageManager> manager, ColumnDescriptors const & descriptors)
	: m_pageManager(manager)
	, m_columnDescriptors(descriptors)
{
	auto firstPage = m_pageManager->AllocatePage().lock();
	m_firstPageID = firstPage->GetID();
	firstPage->SetNextPageID(INVALID_PAGE_ID);
	firstPage->SetPrevPageID(firstPage->GetID());
	m_pageWithSpace = std::make_unique<DataPage>(*m_pageManager, m_firstPageID, m_columnDescriptors);
	firstPage->SetDirty();
}

Table::Table(std::shared_ptr<PageManager> manager, ColumnDescriptors const & descriptors, PageID firstPage)
	: m_pageManager(manager)
	, m_columnDescriptors(descriptors)
	, m_firstPageID(firstPage)
{
	PageID lastPageID = m_pageManager->GetPage(firstPage).lock()->GetPrevPageID();
	m_pageWithSpace = std::make_unique<DataPage>(*m_pageManager, lastPageID, m_columnDescriptors);
}

Table Table::Deserialize(Page const & page, std::shared_ptr<PageManager> manager) {
	char const * data = page.GetData();

	uint32_t fieldsCount = 0;
	BytesToNumber(data, fieldsCount);
	data += 4;
	std::vector<ColumnDescriptor> descriptors;
	for (uint32_t i = 0; i < fieldsCount; ++i) {
		ColumnDescriptor descriptor = ColumnDescriptor::Deserialize(data);
		descriptors.emplace_back(descriptor);
		data += ColumnDescriptor::DESCRIPTOR_SIZE;
	}

	PageID firstPageID = 0;
	BytesToNumber(data, firstPageID);

	return Table(manager, descriptors, firstPageID);
}

void Table::Serialize(Page & page) const {
	char * data = page.GetData();

	uint32_t descriptorCount = m_columnDescriptors.size();
	NumberToBytes(descriptorCount, data);
	data += 4;
	for (size_t i = 0; i < m_columnDescriptors.size(); ++i) {
		m_columnDescriptors[i].Serialize(data);
		data += ColumnDescriptor::DESCRIPTOR_SIZE;
	}

	NumberToBytes(m_firstPageID, data);
	data += sizeof(m_firstPageID);
}

ColumnDescriptors const & Table::GetDescription() const {
	return m_columnDescriptors;
}

bool Table::Insert(std::vector<std::string> const & columns, Values const & values) {
	if (columns.size() != values.size() && m_columnDescriptors.size() != values.size()) {
		Log(LogType::Error) << "Trying to insert " << values.size() << " values"
							<< " into " << columns.size() << " columns. "
							<< "Table has " << m_columnDescriptors.size() << " columns."
							<< std::endl;
		throw std::runtime_error("Different number of values and columns.");
	}

	if (!m_pageWithSpace->HasFreeSpace())
		AddPage();

	std::map<std::string, Value> colVals;
	if (columns.empty())
		for (size_t i = 0; i < m_columnDescriptors.size(); ++i) {
			auto const & desc = m_columnDescriptors[i];
			colVals[desc.name] = values[i];
		}
	else {
		for (auto const & descriptor: m_columnDescriptors)
			colVals[descriptor.name] = Value(descriptor.type, std::string());
		for (size_t i = 0; i < columns.size(); ++i)
			colVals[columns[i]] = values[i];
	}

	// TODO: Insert into indices too

	return m_pageWithSpace->AppendRecord(colVals);
}

std::unique_ptr<DeleteCursor> Table::GetDeleteCursorByType(CursorType type) {
	switch(type) {
	case FULL_SCAN:
	case INDEX:
	default:
		return std::make_unique<FullScanCursor>(*m_pageManager, m_firstPageID, m_columnDescriptors);
	}
}

bool Table::HasIndex(std::string const& column) const {
	return false;
}

void Table::AddPage() {
	auto firstPage = m_pageManager->GetPage(m_firstPageID).lock();
	auto lastPage = m_pageManager->GetPage(firstPage->GetPrevPageID()).lock();
	auto newPage = m_pageManager->AllocatePage().lock();

	newPage->SetNextPageID(lastPage->GetNextPageID());
	newPage->SetPrevPageID(lastPage->GetID());
	PageID newPageID = newPage->GetID();
	firstPage->SetPrevPageID(newPageID);
	lastPage->SetNextPageID(newPageID);

	m_pageWithSpace = std::make_unique<DataPage>(*m_pageManager, newPageID, m_columnDescriptors);
}
