#include <backend/PageManager.h>
#include <utils/Log.h>
#include <utils/Utils.h>

#include "Table.h"
#include "FullScanCursor.h"

using std::uint32_t;

// ToDo: replace all magic 4 number to sizeof(something)
Table::Table(std::shared_ptr<PageManager> manager, ColumnDescriptors const & descriptors)
	: m_pageManager(manager)
	, m_columnDescriptors(descriptors)
	, m_indices()
{
	auto firstPage = m_pageManager->AllocatePage().lock();
	m_firstPageID = firstPage->GetID();
	firstPage->SetNextPageID(INVALID_PAGE_ID);
	firstPage->SetPrevPageID(firstPage->GetID());
	m_pageWithSpace = std::make_unique<DataPage>(*m_pageManager, m_firstPageID, m_columnDescriptors);
	firstPage->SetDirty();
}

Table::Table(std::shared_ptr<PageManager> manager, ColumnDescriptors const & columnDescriptors, PageID firstPage)
	: m_columnDescriptors(columnDescriptors)
	, m_indices(indices)
	, m_pageManager(pageManager)
	, m_firstPageID(firstPage)
{
	PageID lastPageID = m_pageManager->GetPage(firstPage).lock()->GetPrevPageID();
	m_pageWithSpace = std::make_unique<DataPage>(*m_pageManager, lastPageID, m_columnDescriptors);
}

bool Table::AddBTreeIndex(std::string const & name, ColumnDescriptor const & column) {
	auto index = std::make_unique<Index>(m_pageManager, name, column);
	auto cursor = GetFullScanCursor();

	while(cursor->Next())
	{
		auto const & record = cursor->Get(column.name);
	}
	m_indices[name] = std::move(index);

	return true;
}

Table Table::Deserialize(Page const & page, std::shared_ptr<PageManager> manager) {
	auto data = page.GetData();

	uint32_t fieldsCount = 0;
	BytesToNumber(data, fieldsCount);
	data += 4;
	ColumnDescriptors descriptors;
	for (size_t i = 0; i < fieldsCount; ++i) {
		auto descriptor = ColumnDescriptor::Deserialize(data);
		descriptors.emplace_back(descriptor);
		data += ColumnDescriptor::DESCRIPTOR_SIZE;
	}

	uint32_t indicesCount = 0;
	BytesToNumber(data, indicesCount);
	Indices indices;
	for (size_t i = 0; i < indicesCount; ++i) {
		auto index = Index::Deserialize(data, descriptors, manager);
		indices[index->GetName()] = std::move(index);
		data += INDEX_SIZE;
	}

	PageID firstPageID = 0;
	BytesToNumber(data, firstPageID);

	return Table(manager, descriptors, indices, firstPageID);
}

void Table::Serialize(Page & page) const {
	auto data = page.GetData();

	auto descriptorCount = m_columnDescriptors.size();
	NumberToBytes(descriptorCount, data);
	data += 4;
	for (size_t i = 0; i < m_columnDescriptors.size(); ++i) {
		m_columnDescriptors[i].Serialize(data);
		data += ColumnDescriptor::DESCRIPTOR_SIZE;
	}

	auto indicesCount = m_indices.size();
	NumberToBytes(indicesCount, data);
	data += 4;
	for (auto const & ix_kv : m_indices)
	{
		ix_kv.second->Serialize(data);
		data += INDEX_SIZE;
	}

	NumberToBytes(m_firstPageID, data);
	data += sizeof(m_firstPageID);
}

ColumnDescriptors const & Table::GetDescription() const {
	return m_columnDescriptors;
}

bool Table::HasIndex(std::string const & name) {
	return m_indices.find(name) != m_indices.end();
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

	return m_pageWithSpace->AppendRecord(colVals);
}

std::unique_ptr<ICursor> Table::GetCursor(Conditions const & conditions) {
	return std::make_unique<FullScanCursor>(*m_pageManager, m_firstPageID, m_columnDescriptors, conditions);
}

std::unique_ptr<FullScanCursor> Table::GetFullScanCursor()
{
	return std::make_unique<FullScanCursor>(m_pageManager, m_firstPageID, m_columnDescriptors);
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
