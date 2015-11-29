#ifndef MyDB_h
#define MyDB_h

#include <string>
#include <map>
#include <memory>

#include <backend/PageManager.h>
#include <sql/SQLStatement.h>

#include "Table.h"

class MyDB {
public:
	MyDB(std::string const & filename);
	~MyDB();

	void ExecuteStatement(std::unique_ptr<ISQLStatement> statement);

private:
	void ExecuteCreateStatement(CreateStatement const & statement);
	void ExecuteInsertStatement(InsertStatement const & statement);
	void ExecuteSelectStatement(SelectStatement const & statement);
	void LoadTables();
	void SaveTables();

private:
	std::unique_ptr<PageManager> m_pageManager;
	std::map<std::string, std::unique_ptr<Table>> m_tables;

	static PageID constexpr SYSTEM_PAGE_ID = 0;
};

#endif // MyDB_h
