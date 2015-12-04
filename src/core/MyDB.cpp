#include <experimental/filesystem>
#include <cstdint>

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

void MyDB::ExecuteStatement(std::unique_ptr<ISQLStatement> statement) {
	switch (statement->GetType()) {
		case SQLStatementType::CREATE: {
			auto const & create = static_cast<CreateStatement &>(*statement);
			ExecuteCreateStatement(create);
			break;
		}
		case SQLStatementType::INSERT: {
			auto const & insert = static_cast<InsertStatement &>(*statement);
			ExecuteInsertStatement(insert);
			break;
		}
		case SQLStatementType::SELECT: {
			auto const & select = static_cast<SelectStatement &>(*statement);
			ExecuteSelectStatement(select);
			break;
		}
	}
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
	char * data = m_pageManager->GetPage(SYSTEM_PAGE_ID).lock()->GetData();

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
	}
}

void MyDB::ExecuteCreateStatement(CreateStatement const & statement) {
}

void MyDB::ExecuteInsertStatement(InsertStatement const & statement) {
}

void MyDB::ExecuteSelectStatement(SelectStatement const & statement) {
}
