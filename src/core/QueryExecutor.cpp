#include <cassert>

#include "QueryExecutor.h"

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
	do {
		cursor->Next();
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
	} while (cursor->HasNext());

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

bool QueryExecutor::ExecuteCreateTableStatement(CreateTableStatement const& statement, std::map<std::string, Table> & tables, std::shared_ptr<PageManager> pageManager) const
{
	auto const & columns = statement.GetColumns();
	size_t headerSize = 0;
	for (auto const & column : columns) {
		size_t nameLen = strlen(column.name);
		if (nameLen > COLUMN_NAME_LENGTH)
			throw std::runtime_error("Column name should be less than " +
				std::to_string(COLUMN_NAME_LENGTH) + "symbols.");
		headerSize += ColumnDescriptor::DESCRIPTOR_SIZE;
	}
	if (headerSize > Page::PAGE_DATA_SIZE)
		throw std::runtime_error("Too big record length.");

	std::string const & tableName = statement.GetTableName();
	if (tables.find(tableName) != tables.end())
		throw std::runtime_error("Table with name \"" + tableName + "\" already exist.");

	tables.emplace(tableName, Table(pageManager, columns));

	return true;
}

bool QueryExecutor::ExecuteCreateIndexStatement(CreateIndexStatement const& statement, Table & table) const
{
	assert(false && "Create index not implemented.");
	return false;
}

bool QueryExecutor::ExecuteInsertStatement(InsertStatement const& statement, Table & table) const
{
	return table.Insert(statement.GetColumns(), statement.GetValues());
}

std::unique_ptr<Cursor> QueryExecutor::ExecuteSelectStatement(SelectStatement const& statement, Table const& table)
{
	return GetCursor(table, statement.GetConditions());
}

// TODO: Remove duplicated code
std::unique_ptr<DeleteCursor> QueryExecutor::GetDeleteCursor(Table & table, Conditions const & conditions) const
{
	for (auto const & condition : conditions) {
		if (table.HasIndex(condition.GetColumn())){
			return table.GetDeleteCursorByType(INDEX, conditions);
		}
	}

	return table.GetDeleteCursorByType(FULL_SCAN, conditions);
}

std::unique_ptr<Cursor> QueryExecutor::GetCursor(Table const & table, Conditions const & conditions) const
{
	for (auto const & condition : conditions) {
		if (table.HasIndex(condition.GetColumn())) {
			return table.GetCursorByType(INDEX, conditions);
		}
	}

	return table.GetCursorByType(FULL_SCAN, conditions);
}
