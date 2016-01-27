#include <experimental/filesystem>
#include <cstdint>
#include <cassert>

#include <utils/Utils.h>

#include "MyDB.h"

using std::uint32_t;

MyDB::MyDB(std::string const & filename) 
	: m_executor()
{
	bool fileExists = std::experimental::filesystem::exists(filename);
	m_pageManager = std::make_unique<PageManager>(filename, fileExists);
	if (fileExists)
		LoadTables();
}

MyDB::~MyDB() {
	SaveTables();
}

ColumnDescriptors const & MyDB::GetTableDescription(std::string const & tableName) const {
	return const_cast<MyDB &>(*this).FindTable(tableName).GetDescription();
}

bool MyDB::ExecuteCreate(std::unique_ptr<ISQLStatement> const & statement) {
	assert(statement);

	switch (statement->GetType()) {
		case SQLStatementType::CREATE_TABLE: {
			auto const & create = static_cast<CreateTableStatement &>(*statement);
			return m_executor.ExecuteCreateTableStatement(create, m_tables, m_pageManager);
		}
		case SQLStatementType::CREATE_INDEX: {
			auto const & create = static_cast<CreateIndexStatement &>(*statement);
			return m_executor.ExecuteCreateIndexStatement(create, FindTable(create.GetName()));
		}
		default:
			throw std::runtime_error("Unknown create statement.");
	}
}

size_t MyDB::ExecuteUpdate(std::unique_ptr<ISQLStatement> const & statement) {
	assert(statement);

	switch (statement->GetType()) {
		case SQLStatementType::INSERT: {
			auto const & insert = static_cast<InsertStatement &>(*statement);
			return m_executor.ExecuteInsertStatement(insert, FindTable(insert.GetTableName())) ? 1 : 0;
		}
		case SQLStatementType::UPDATE: {
			auto const & update = static_cast<UpdateStatement &>(*statement);
			return m_executor.ExecuteUpdateStatement(update, FindTable(update.GetTableName()));
		}
		case SQLStatementType::DELETE: {
			auto const & del = static_cast<DeleteStatement &>(*statement);
			return m_executor.ExecuteDeleteStatement(del, FindTable(del.GetTableName()));
		}
		default:
			throw std::runtime_error("Unknown update statement.");
	}
}

std::unique_ptr<Cursor> MyDB::ExecuteQuery(std::unique_ptr<ISQLStatement> const & statement) {
	assert(statement);

	if (SQLStatementType::SELECT != statement->GetType())
		throw std::runtime_error("Can't execute non select query.");

	auto const & select = static_cast<SelectStatement const &>(*statement);
	auto & table = FindTable(select.GetTableName());
	return m_executor.ExecuteSelectStatement(select, table);
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
		data += sizeof(PageID);
		auto tablePage = m_pageManager->GetPage(tablePageID).lock();
		m_tables.emplace(tableName, Table::Deserialize(*tablePage, m_pageManager));
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
		data += sizeof(PageID);
		auto & table = nameTable.second;
		table.Serialize(*tablePage);
		tablePage->SetDirty();
	}
	systemPage->SetDirty();
}

Table & MyDB::FindTable(std::string const & name) {
	auto it = m_tables.find(name);
	if (m_tables.end() == it)
		throw std::runtime_error("Table with name \"" + name + "\" does not exist.");
	return it->second;
}
