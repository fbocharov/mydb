#include <experimental/filesystem>
#include <cstdint>
#include <cassert>

#include <utils/Utils.h>

#include "MyDB.h"

using std::uint32_t;

MyDB::MyDB(std::string const & filename) {
	bool fileExists = std::experimental::filesystem::exists(filename);
	m_pageManager = std::make_unique<PageManager>(filename, fileExists);
	if (fileExists)
		LoadTables();
}

MyDB::~MyDB() {
	SaveTables();
}

ColumnDescriptors const & MyDB::GetTableDescription(std::string const & tableName) const {
	return FindTable(tableName).GetDescription();
}


bool MyDB::ExecuteCreate(std::unique_ptr<ISQLStatement> const & statement) {
	// TODO: add create index statement here.
	switch (statement->GetType()) {
		case SQLStatementType::CREATE: {
			auto const & create = static_cast<CreateStatement &>(*statement);
			return ExecuteCreateStatement(create);
		}
		default:
			throw std::runtime_error("Unknown create statement.");
	}
}

size_t MyDB::ExecuteUpdate(std::unique_ptr<ISQLStatement> const & statement) {
	// TODO: add update and delete statements here.
	switch (statement->GetType()) {
		case SQLStatementType::INSERT: {
			auto const & insert = static_cast<InsertStatement &>(*statement);
			return ExecuteInsertStatement(insert) ? 1 : 0;
		}
		default:
			throw std::runtime_error("Unknown update statement.");
	}
}

std::unique_ptr<ICursor> MyDB::ExecuteQuery(std::unique_ptr<ISQLStatement> const & statement) {
	if (SQLStatementType::SELECT != statement->GetType())
		throw std::runtime_error("Can't execute non select query.");

	auto & table = FindTable(statement->GetTableName());
	return table.GetCursor();
}

void MyDB::LoadTables() {
	char const * data = m_pageManager->GetPage(SYSTEM_PAGE_ID).lock()->GetData();

	uint32_t tablesCount;
	BytesToNumber(data, tablesCount);
	data += 4;

	for (size_t i = 0; i < tablesCount; ++i) {
		uint32_t nameLen = 0;
		BytesToNumber(data, nameLen);
		data += 4;

		std::string tableName(data, nameLen);
		data += nameLen;

		PageID tablePageID;
		BytesToNumber(data, tablePageID);
		auto tablePage = m_pageManager->GetPage(tablePageID).lock();
		m_tables[tableName] = Table::Deserialize(*tablePage, *m_pageManager);
		m_pageManager->DeallocatePage(tablePage->GetID());
	}
}

void MyDB::SaveTables() {
	auto systemPage = m_pageManager->GetPage(SYSTEM_PAGE_ID).lock();
	char * data = systemPage->GetData();

	uint32_t tablesCount = m_tables.size();
	NumberToBytes(tablesCount, data);
	data += 4;

	for (auto const & nameTable: m_tables) {
		std::string const name = nameTable.first;
		uint32_t nameLen = name.length();
		NumberToBytes(nameLen, data);
		data += 4;

		memcpy(data, name.data(), nameLen);
		data += nameLen;

		auto tablePage = m_pageManager->AllocatePage().lock();
		NumberToBytes(tablePage->GetID(), data);
		auto const & table = nameTable.second;
		table->Serialize(*tablePage);
		tablePage->SetDirty();
	}
	systemPage->SetDirty();
}

bool MyDB::ExecuteCreateStatement(CreateStatement const & statement) {
	auto const & columns = statement.GetColumns();
	for (auto const & column: columns)
		if (strlen(column.name) > COLUMN_NAME_LENGTH)
			throw std::runtime_error("Column name should be less than " +
				std::to_string(COLUMN_NAME_LENGTH) + "symbols.");

	m_tables[statement.GetTableName()] = std::make_unique<Table>(*m_pageManager, columns);
	return true;
}

bool MyDB::ExecuteInsertStatement(InsertStatement const & statement) {
	auto & table = FindTable(statement.GetTableName());
	return table.Insert(statement.GetColumns(), statement.GetValues());
}

Table & MyDB::FindTable(std::string const & name) const {
	auto it = m_tables.find(name);
	if (m_tables.end() == it)
		throw std::runtime_error("Table with name " + name + " does not exist.");
	return *it->second;
}
