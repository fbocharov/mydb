#ifndef QueryExecutor_h
#define QueryExecutor_h
#include "Table.h"
#include "DeleteCursor.h"
#include <sql/SQLStatement.h>


class QueryExecutor {
public:
	QueryExecutor();

	bool ExecuteCreateTableStatement(CreateTableStatement const& statement, 
			std::map<std::string, Table> & tables, std::shared_ptr<PageManager> pageManager) const;
	bool ExecuteCreateIndexStatement(CreateIndexStatement const & statement, Table & table) const;
	
	size_t ExecuteUpdateStatement(UpdateStatement const & statement, Table & table) const;
	size_t ExecuteDeleteStatement(DeleteStatement const & statement, Table & table) const;
	bool ExecuteInsertStatement(InsertStatement const & statement, Table & table) const;

	std::unique_ptr<Cursor> ExecuteSelectStatement(SelectStatement const & statement, Table const & table);

	~QueryExecutor();
private:
	std::unique_ptr<DeleteCursor> GetDeleteCursor(Table & table, Conditions const & conditions) const;
	std::unique_ptr<Cursor> GetCursor(Table const & table, Conditions const & conditions) const;
};

#endif // QueryExecutor_h
