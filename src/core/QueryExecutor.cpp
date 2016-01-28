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
	bool inLeft, inRight;

	Conditions leftConditions;
	Conditions rightConditions;

	auto const & leftName = leftTable.GetName();
	auto const & rightName = rightTable.GetName();
	for (auto const & condition : statement.GetConditions()) {
		auto const splitted = SplitQualified(condition.GetColumn());
		inLeft = inRight = false;
		if (splitted.first == "" || splitted.first == leftName)
			inLeft = leftTable.HasDescriptor(splitted.second);
		if (splitted.first == "" || splitted.first == rightName)
			inRight = rightTable.HasDescriptor(splitted.second);
		if(inLeft == inRight) {
			std::string msg = inLeft == true
				? "Ambigious conditions \"" + condition.GetColumn() + "\" with operation " + condition.GetOperation()
				: "Cannot resolve condition \"" + condition.GetColumn() + "\" with operation " + condition.GetOperation();
			throw std::runtime_error(msg);
		}

		if (inLeft)
			leftConditions.push_back(condition.CopyWithNewColName(splitted.second));
		else
			rightConditions.push_back(condition.CopyWithNewColName(splitted.second));
	}

	std::vector<std::string> leftFields;
	std::vector<std::string> rifghtFields;
	auto splitted = SplitQualifiedVectorUnzip(statement.GetFields());
	for (size_t i = 0; i < splitted.first.size(); ++i) {
		auto const & tableName = splitted.first[i];
		auto const & columnName = splitted.second[i];
		inLeft = inRight = false;
		if (tableName == "" || tableName == leftName)
			inLeft = leftTable.HasDescriptor(columnName);
		if (tableName == "" || tableName == rightName)
			inRight = rightTable.HasDescriptor(columnName);

		if (inLeft == inRight) {
			std::string msg = inLeft == true
				? "Ambigious field names \"" + columnName
				: "Cannot resolve names \"" + tableName + "." + columnName;
			throw std::runtime_error(msg);
		}

		if (inLeft)
			leftFields.push_back(columnName);
		else
			rifghtFields.push_back(columnName);
	}

	auto leftPlannedCursor = PlaneQuery(leftTable, leftConditions);
	auto rightPlannedCursor = PlaneQuery(rightTable, rightConditions);

	std::unique_ptr<ICursor> leftFilteredCursor = leftConditions.empty()
		? std::move(leftPlannedCursor)
		: std::make_unique<FilterCursor>(std::move(leftPlannedCursor), leftConditions);

	std::unique_ptr<ICursor> rightFilteredCursor = rightConditions.empty()
		? std::move(rightPlannedCursor)
		: std::make_unique<FilterCursor>(std::move(rightPlannedCursor), rightConditions);

	auto leftProjectionCursor = leftFields.empty()
		? std::move(leftFilteredCursor)
		: std::make_unique<ProjectionCursor>(std::move(leftFilteredCursor), leftFields);

	auto rightProjectionCursor = rifghtFields.empty()
		? std::move(rightFilteredCursor)
		: std::make_unique<ProjectionCursor>(std::move(rightFilteredCursor), rifghtFields);

	inLeft = inRight = false;
	
	auto const & joinFields = statement.GetJoinFields();
	auto const firstSplitted = SplitQualified(joinFields.first);
	auto const secondSplitted = SplitQualified(joinFields.second);

	return std::make_unique<JoinCursor>(move(leftProjectionCursor), move(rightProjectionCursor), firstSplitted.second, secondSplitted.second);
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
