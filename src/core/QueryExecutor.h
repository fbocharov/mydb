#ifndef QueryExecutor_h
#define QueryExecutor_h

#include <sql/SQLStatement.h>

#include "Table.h"
#include "ICursor.h"

class QueryExecutor {
public:
	size_t ExecuteUpdateStatement(UpdateStatement const & statement, Table & table) const;
	size_t ExecuteDeleteStatement(DeleteStatement const & statement, Table & table) const;
	bool ExecuteInsertStatement(InsertStatement const & statement, Table & table) const;

	std::unique_ptr<ICursor> ExecuteSelectStatement(SelectStatement const & statement, Table & table);

private:
	std::unique_ptr<DeleteCursor> GetDeleteCursor(Table & table, Conditions const & conditions) const;
};

#endif // QueryExecutor_h
