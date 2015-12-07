#ifndef MyDB_h
#define MyDB_h

#include <string>
#include <map>
#include <memory>

#include <backend/PageManager.h>
#include <sql/SQLStatement.h>

#include "Table.h"
#include "Cursor.h"

class MyDB {
public:
	MyDB(std::string const & filename);
	~MyDB();

	bool ExecuteCreate(std::unique_ptr<ISQLStatement> statement);
	size_t ExecuteUpdate(std::unique_ptr<ISQLStatement> statement);
	std::unique_ptr<ICursor> ExecuteQuery(std::unique_ptr<ISQLStatement> statement);

private:
	bool ExecuteCreateStatement(CreateStatement const & statement);
	bool ExecuteInsertStatement(InsertStatement const & statement);
	Table & FindTable(std::string const & name);
	void LoadTables();
	void SaveTables();

private:
	std::unique_ptr<PageManager> m_pageManager;
	std::map<std::string, std::unique_ptr<Table>> m_tables;

	static PageID constexpr SYSTEM_PAGE_ID = 0;
};

#endif // MyDB_h
