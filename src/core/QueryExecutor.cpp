#include <algorithm>

#include "QueryExecutor.h"
#include "FilterCursor.h"
#include "ProjectionCursor.h"
#include "JoinCursor.h"

size_t QueryExecutor::ExecuteUpdateStatement(UpdateStatement const & statement, Table & table) const
{

	auto const & conditions = statement.GetConditions();
	auto cursor = std::make_unique<FilterCursor>(table.GetFullScanCursor(), conditions);

	auto const & updated = statement.GetColVals();
	auto const & descs = table.GetDescription();
	for (auto const & cond: conditions) {
		auto column = cond.GetColumn();
		if (!table.HasIndex(column))
			continue;

		auto it = std::find_if(updated.begin(), updated.end(),
			[column] (std::pair<std::string, Value> const & p) {
				return column == p.first;
			}
		);
		if (updated.end() == it) {
			// Table has index on this column and this column is not updated -> better use index cursor.
			auto internalCursor = GetIndexCursor(table, conditions, cond);
			cursor = std::make_unique<FilterCursor>(std::move(internalCursor), conditions);
			break;
		}
	}
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

		cursor->Delete();
		table.Insert(colNames, values);
		++updatedCount;
	}

	return updatedCount;
}

size_t QueryExecutor::ExecuteDeleteStatement(DeleteStatement const& statement, Table & table) const
{
	auto const & conditions = statement.GetConditions();
	auto plannedCursor = PlaneQuery(table, conditions);
	auto cursor = conditions.empty()
		? std::move(plannedCursor)
		: std::make_unique<FilterCursor>(std::move(plannedCursor), conditions);
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

	std::vector<std::pair<int, std::string>> joinFields;
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
			joinFields.push_back(std::make_pair(1, columnName));
		else
			joinFields.push_back(std::make_pair(2, columnName));
	}

	auto leftPlannedCursor = PlaneQuery(leftTable, leftConditions);
	auto rightPlannedCursor = PlaneQuery(rightTable, rightConditions);

	std::unique_ptr<ICursor> leftFilteredCursor = leftConditions.empty()
		? std::move(leftPlannedCursor)
		: std::make_unique<FilterCursor>(std::move(leftPlannedCursor), leftConditions);

	std::unique_ptr<ICursor> rightFilteredCursor = rightConditions.empty()
		? std::move(rightPlannedCursor)
		: std::make_unique<FilterCursor>(std::move(rightPlannedCursor), rightConditions);

	auto const & joinOn = statement.GetJoinFields();
	auto const firstSplitted = SplitQualified(joinOn.first);
	auto const secondSplitted = SplitQualified(joinOn.second);

	bool firstInLeft, firstInRight;
	firstInLeft = firstInRight = false;

	if (firstSplitted.first == "" || firstSplitted.first == leftName)
		firstInLeft = leftTable.HasDescriptor(firstSplitted.second);
	if (firstSplitted.first == "" || firstSplitted.first == rightName)
		firstInRight = rightTable.HasDescriptor(firstSplitted.second);
	if(firstInLeft == firstInRight) {
		auto msg = firstInLeft == true
			? "Ambigious on predicate \"" + firstSplitted.second + "\""
			: "Cannot resolve on predicate \"" + firstSplitted.first + (firstSplitted.first == "" ? "" : ".") + firstSplitted.second + "\"";
		throw std::runtime_error(msg);
	}

	bool secondInLeft, secondInRight;
	secondInLeft = secondInRight = false;

	if (secondSplitted.first == "" || secondSplitted.first == leftName)
		secondInLeft = leftTable.HasDescriptor(secondSplitted.second);
	if (secondSplitted.first == "" || secondSplitted.first == rightName)
		secondInRight = rightTable.HasDescriptor(secondSplitted.second);

	if (secondInLeft == secondInRight) {
		auto msg = secondInRight == true
			? "Ambigious on predicate \"" + secondSplitted.second + "\""
			: "Cannot resolve on predicate \"" + secondSplitted.first + (secondSplitted.first == "" ? "" : ".") + secondSplitted.second + "\"";
		throw std::runtime_error(msg);
	}

	if (firstInLeft == secondInLeft)
		throw std::runtime_error("Wrong on preadicate in join statement. Use field from different tables.");

	if(firstInLeft)
		return std::make_unique<JoinCursor>(move(leftFilteredCursor), move(rightFilteredCursor), firstSplitted.second, secondSplitted.second, joinFields);
	else
		return std::make_unique<JoinCursor>(move(leftFilteredCursor), move(rightFilteredCursor), secondSplitted.second, firstSplitted.second, joinFields);
}

std::unique_ptr<InternalCursor> QueryExecutor::PlaneQuery(Table& table, Conditions const& conditions) const
{
	for (auto const & condition : conditions)
		if (table.HasIndex(condition.GetColumn()))
			return GetIndexCursor(table, conditions, condition);

	return table.GetFullScanCursor();
}

std::unique_ptr<InternalCursor> QueryExecutor::GetIndexCursor(Table const & table,
		Conditions const & conditions, Condition const & first) const
{
	auto op = first.GetOperation();
	if (op != '=') {
		auto inverted = op == '>' ? '<' : '>';
		for (auto const & secondCondition : conditions) {
			if (secondCondition.GetColumn() == first.GetColumn()
				&& secondCondition.GetOperation() == inverted) {
				return op == '<'
					? table.GetIndexCursor(secondCondition, first)
					: table.GetIndexCursor(first, secondCondition);
			}
		}
	}
	return table.GetIndexCursor(first, first);
}

