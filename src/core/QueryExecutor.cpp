#include "QueryExecutor.h"
#include "FilterCursor.h"

size_t QueryExecutor::ExecuteUpdateStatement(UpdateStatement const & statement, Table & table) const
{
	auto cursor = GetDeleteCursor(table, statement.GetConditions());

	auto const & updated = statement.GetColVals();
	auto descs = table.GetDescription();
	std::vector<std::string> colNames;

	// TODO: move column names receiving to public interface of Table class
	for (auto const & d : descs) {
		colNames.push_back(d.name);
	}

	size_t updatedCount = 0;
	while (cursor->Next()) {
		auto values = cursor->GetAll();
		for (auto const & kv : updated) {
			for (size_t i = 0; i < colNames.size(); ++i) {
				if (kv.first == colNames[i]) {
					values[i] = kv.second;
					break;
				}
			}
		}
		
		table.Insert(colNames, values);
		cursor->Delete();
		++updatedCount;
	}

	return updatedCount;
}

size_t QueryExecutor::ExecuteDeleteStatement(DeleteStatement const& statement, Table & table) const
{
	auto cursor = GetDeleteCursor(table, statement.GetConditions());
	size_t deleted = 0;
	while (cursor->Next())
		if (cursor->Delete())
			++deleted;
	return deleted;
}

bool QueryExecutor::ExecuteInsertStatement(InsertStatement const& statement, Table & table) const
{
	return table.Insert(statement.GetColumns(), statement.GetValues());
}

std::unique_ptr<ICursor> QueryExecutor::ExecuteSelectStatement(SelectStatement const& statement, Table & table)
{
	return GetDeleteCursor(table, statement.GetConditions());
}

std::unique_ptr<DeleteCursor> QueryExecutor::GetDeleteCursor(Table & table, Conditions const & conditions) const
{
	for (auto const & condition : conditions) {
		if (table.HasIndex(condition.GetColumn())){
			return std::make_unique<FilterCursor>(table.GetCursorByType(INDEX), conditions);
		}
	}

	if(conditions.size() == 0)
	{
		return table.GetCursorByType(FULL_SCAN);
	}

	return std::make_unique<FilterCursor>(table.GetCursorByType(FULL_SCAN), conditions);
}
