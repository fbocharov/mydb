#include "QueryExecutor.h"
#include "FilterCursor.h"
#include "ProjectionCursor.h"
#include "JoinCursor.h"

size_t QueryExecutor::ExecuteUpdateStatement(UpdateStatement const & statement, Table & table) const
{
	auto cursor = GetCursor(table, statement.GetConditions());

	auto const & updated = statement.GetColVals();
	auto const & descs = table.GetDescription();
	std::vector<std::string> colNames;

	// TODO: move column names receiving to public interface of Table class
	for (auto const & d : descs)
		colNames.push_back(d.name);

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
	auto cursor = GetCursor(table, statement.GetConditions());
	size_t deleted = 0;
	while (cursor->Next())
		if (cursor->Delete())
			++deleted;
	return deleted;
}

bool QueryExecutor::ExecuteInsertStatement(InsertStatement const& statement, Table & table) const
{
	return table.Insert(statement.GetFields(), statement.GetValues());
}

std::unique_ptr<ICursor> QueryExecutor::ExecuteSelectStatement(SelectStatement const& statement, Table & table)
{
	Conditions conditions;
	for (auto & condition : statement.GetConditions()) {
		auto const splited = SplitQualified(condition.GetColumn());
		conditions.push_back(condition.CopyWithNewColName(splited.second));
	}

	auto plannedCursor = PlaneQuery(table, conditions);
	auto filteredCursor = conditions.empty()
		? std::move(plannedCursor)
		: std::make_unique<FilterCursor>(move(plannedCursor), conditions);

	auto fields = SplitQualifiedVectorUnzip(statement.GetFields());
	if (fields.second.empty())
		return std::move(filteredCursor);

	return std::make_unique<ProjectionCursor>(std::move(filteredCursor), fields.second);
}

std::unique_ptr<ICursor> QueryExecutor::ExecuteJoinStatement(JoinStatement const& statement, Table& leftTable, Table& rightTable)
{
	auto leftCursor  = leftTable.GetFullScanCursor();
	auto rightCursor = rightTable.GetFullScanCursor();
	auto joinFields = statement.GetJoinFields();
	return std::make_unique<JoinCursor>(move(leftCursor), move(rightCursor), joinFields.first, joinFields.second);
}

std::unique_ptr<InternalCursor> QueryExecutor::PlaneQuery(Table& table, Conditions const& conditions) const
{
	for (auto const & condition : conditions) {
		if (table.HasIndex(condition.GetColumn())) {
			auto op = condition.GetOperation();
			if (op != '=') {
				auto inverted = op == '>' ? '<' : '>';
				for (auto const & secondCondition : conditions) {
					if (secondCondition.GetColumn() == condition.GetColumn()
						&& secondCondition.GetOperation() == inverted) {
						return op == '<'
							? table.GetIndexCursor(secondCondition, condition)
							: table.GetIndexCursor(condition, secondCondition);
					}
				}
			}

			return table.GetIndexCursor(condition, condition);
		}
	}

	return table.GetFullScanCursor();
}

std::unique_ptr<InternalCursor> QueryExecutor::GetCursor(Table & table, Conditions const & conditions) const
{
	for (auto const & condition : conditions) {
		if (table.HasIndex(condition.GetColumn())) {
			auto op = condition.GetOperation();
			if (op != '=') {
				auto inverted = op == '>' ? '<' : '>';
				for (auto const & secondCondition : conditions)
				{
					if (secondCondition.GetColumn() == condition.GetColumn() 
						&& secondCondition.GetOperation() == inverted) {
						return op == '<'
							? std::make_unique<FilterCursor>(table.GetIndexCursor(secondCondition, condition), conditions)
							: std::make_unique<FilterCursor>(table.GetIndexCursor(condition, secondCondition), conditions);
					}
				}
			}

			return std::make_unique<FilterCursor>(table.GetIndexCursor(condition, condition), conditions);
		}
	}
		

	if(conditions.empty())
		return table.GetFullScanCursor();

	return std::make_unique<FilterCursor>(table.GetFullScanCursor(), conditions);
}
