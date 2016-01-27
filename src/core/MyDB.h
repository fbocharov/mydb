#ifndef MyDB_h
#define MyDB_h

#include <string>
#include <map>
#include <memory>

#include <backend/PageManager.h>
#include <sql/SQLStatement.h>

#include "Table.h"
#include "QueryExecutor.h"

class MyDB {
public:
	MyDB(std::string const & filename);
	~MyDB();

	ColumnDescriptors const & GetTableDescription(std::string const & tableName) const;
	bool ExecuteCreate(std::unique_ptr<ISQLStatement> const & statement);
	size_t ExecuteUpdate(std::unique_ptr<ISQLStatement> const & statement);
	std::unique_ptr<ICursor> ExecuteQuery(std::unique_ptr<ISQLStatement> const & statement);

private:
	bool ExecuteCreateTableStatement(CreateTableStatement const& statement);
	bool ExecuteCreateIndexStatement(CreateIndexStatement const & statement);

	Table & FindTable(std::string const & name);
	void LoadTables();
	void SaveTables();

private:
	std::shared_ptr<PageManager> m_pageManager;
	std::map<std::string, Table> m_tables;

	QueryExecutor m_executor;

	static PageID constexpr SYSTEM_PAGE_ID = 0;
};

#endif // MyDB_h
