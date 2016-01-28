#include <backend/PageManager.h>
#include <utils/Log.h>
#include <utils/Utils.h>

#include "Table.h"
#include "FullScanCursor.h"

using std::uint32_t;

Table::Table(std::shared_ptr<PageManager> manager, ColumnDescriptors const & descriptors, 
			std::string const & name)
	: m_pageManager(manager)
	, m_columnDescriptors(descriptors)
	, m_name(name)
{
	auto firstPage = m_pageManager->AllocatePage().lock();
	m_firstPageID = firstPage->GetID();
	firstPage->SetNextPageID(INVALID_PAGE_ID);
	firstPage->SetPrevPageID(firstPage->GetID());
	m_pageWithSpace = std::make_unique<DataPage>(*m_pageManager, m_firstPageID, m_columnDescriptors);
	firstPage->SetDirty();
}

Table::Table(std::shared_ptr<PageManager> manager, ColumnDescriptors const & descriptors, 
			PageID firstPage, std::string const & name)
	: m_pageManager(manager)
	, m_columnDescriptors(descriptors)
	, m_firstPageID(firstPage)
	, m_name(name)
{
	PageID lastPageID = m_pageManager->GetPage(firstPage).lock()->GetPrevPageID();
	m_pageWithSpace = std::make_unique<DataPage>(*m_pageManager, lastPageID, m_columnDescriptors);
}

Table Table::Deserialize(Page const & page, std::shared_ptr<PageManager> manager, std::string const & name) {
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
	data += sizeof(firstPageID);

	Table table(manager, descriptors, firstPageID, name);
	table.DeserializeIndices(data);

	return table;
}

std::string const& Table::GetName() const
{
	return m_name;
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

	data = SerializeIndices(data);
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

	auto recordNumber = m_pageWithSpace->GetRecordCount();
	auto pageID = m_pageWithSpace->GetID();
	UpdateIndices(colVals, pageID, recordNumber);

	return m_pageWithSpace->AppendRecord(colVals);
}

bool Table::CreateIndex(std::string const & column, std::string const & name, IndexType type, bool isUnique) {
	std::shared_ptr<Index> index;
	auto const & descriptor = FindDescriptor(column);
	switch (type) {
		case IndexType::BTREE:
			switch (descriptor.type) {
				case ValueType::INT:
					index = std::make_shared<BPlusTreeIndex<int>>(isUnique, m_pageManager);
					break;
				case ValueType::DOUBLE:
					index = std::make_shared<BPlusTreeIndex<double>>(isUnique, m_pageManager);
					break;
				default:
					throw std::runtime_error("Can't create index for this field: unsupported type.");
			}
			break;
		default:
			throw std::runtime_error("Unknown index type.");
	}
	FillIndex(column, index);
	m_indices[name] = index;
	m_columnIndexName[column] = name;

	return true;
}

std::unique_ptr<InternalCursor> Table::GetIndexCursor(Condition const & from, Condition const & to) const
{
	assert(from.GetColumn() == to.GetColumn());

	auto index = FindIndex(from.GetColumn());
	if (!index)
		throw std::runtime_error("Can't create index cursor: no index for column " +
			from.GetColumn());
	auto cursor = index->GetCursor(m_columnDescriptors, from, to);
	cursor->SetTableName(m_name);
	return std::move(cursor);
}

std::unique_ptr<InternalCursor> Table::GetFullScanCursor()
{
	auto cursor = std::make_unique<FullScanCursor>(*m_pageManager, m_firstPageID, m_columnDescriptors);
	cursor->SetTableName(m_name);
	return std::move(cursor);
}

bool Table::HasIndex(std::string const & column) const {
	return nullptr != FindIndex(column);
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

ColumnDescriptor & Table::FindDescriptor(std::string const & name) {
	for (auto & d: m_columnDescriptors)
		if (name == d.name)
			return d;
	throw std::runtime_error("Table doesn't contain column with name \"" + name + "\".");
}

void Table::FillIndex(std::string const & column, std::shared_ptr<Index> & index) {
	auto cursor = GetFullScanCursor();
	while (cursor->Next())
		index->Insert(cursor->Get(column), cursor->GetCurrentPage(), cursor->GetCurrentRecordNumber());
}

std::shared_ptr<Index> Table::FindIndex(std::string const & column) const {
	auto it = m_columnIndexName.find(column);
	if (m_columnIndexName.end() != it)
		return m_indices.find(it->second)->second;
	return nullptr;
}

void Table::UpdateIndices(std::map<std::string, Value> const & colVals, PageID pageID, std::uint32_t recordNumber) {
	for (auto const & columnName: m_columnIndexName) {
		auto const & column = columnName.first;
		auto const & indexName = columnName.second;
		auto index = m_indices[indexName];
		if (colVals.end() != colVals.find(column)) {
			auto const & value = colVals.find(column)->second;
			index->Insert(value, pageID, recordNumber);
		}
	}
}

char * Table::SerializeIndices(char * bytes) const {
	NumberToBytes<std::uint32_t>(m_indices.size(), bytes);
	bytes += sizeof(std::uint32_t);
	for (auto const & columnName: m_columnIndexName) {
		auto const & column = columnName.first;
		auto const & indexName = columnName.second;

		NumberToBytes<std::uint32_t>(column.length(), bytes);
		bytes += sizeof(std::uint32_t);
		memcpy(bytes, column.c_str(), column.length());
		bytes += column.length();

		NumberToBytes<std::uint32_t>(indexName.length(), bytes);
		bytes += sizeof(std::uint32_t);
		memcpy(bytes, indexName.c_str(), indexName.length());
		bytes += indexName.length();

		auto index = m_indices.find(indexName)->second;
		NumberToBytes(index->GetType(), bytes);
		bytes += sizeof(IndexType);

		NumberToBytes<std::uint8_t>(index->IsUnique(), bytes);
		bytes += sizeof(std::uint8_t);

		bytes = index->Serialize(bytes);
	}

	return bytes;
}

void Table::DeserializeIndices(char const * bytes) {
	std::uint32_t indexCount = 0;
	BytesToNumber(bytes, indexCount);
	bytes += sizeof(indexCount);

	for (size_t i = 0; i < indexCount; ++i) {
		std::uint32_t length;

		BytesToNumber(bytes, length);
		bytes += sizeof(length);
		std::string column(bytes, length);
		bytes += length;

		BytesToNumber(bytes, length);
		bytes += sizeof(length);
		std::string indexName(bytes, length);
		bytes += length;

		IndexType type;
		BytesToNumber(bytes, type);
		bytes += sizeof(type);

		std::uint8_t isUnique;
		BytesToNumber(bytes, isUnique);
		bytes += sizeof(isUnique);

		std::shared_ptr<Index> index;
		auto const & descriptor = FindDescriptor(column);
		switch (type) {
			case IndexType::BTREE: {
				switch (descriptor.type) {
					case ValueType::INT:
						index = BPlusTreeIndex<int>::Deserialize(&bytes, isUnique, m_pageManager);
						break;
					case ValueType::DOUBLE:
						index = BPlusTreeIndex<double>::Deserialize(&bytes, isUnique, m_pageManager);
						break;
					default:
						throw std::runtime_error("Table::DeserializeIndex(): unsupported value type.");
				}
				break;
			}
			default:
				throw std::runtime_error("Table::DeserializeIndex(): unknown index type.");
		}
		m_indices[indexName] = index;
		m_columnIndexName[column] = indexName;
	}
}
