#ifndef SQLStatement_h
#define SQLStatement_h

#include <vector>
#include <map>
#include <string>
#include <utility>

#include <common/Common.h>
#include <common/Condition.h>

enum class SQLStatementType {
	CREATE_TABLE,
	CREATE_INDEX,
	SELECT,
	INSERT,
	UPDATE,
	DELETE
};

class ISQLStatement {
public:
	virtual ~ISQLStatement() = default;

	SQLStatementType GetType() const {
		return m_type;
	}

	std::string const & GetTableName() const {
		return m_tableName;
	}

protected:
	explicit ISQLStatement(SQLStatementType type, std::string const & tableName)
		: m_type(type)
		, m_tableName(tableName)
	{}

private:
	SQLStatementType m_type;
	std::string const m_tableName;
};

class WithConditions {
public:
	WithConditions(Conditions const & conditions)
		: m_conditions(conditions)
	{}
	virtual ~WithConditions() = default;

	bool HasConditions() const {
		return !m_conditions.empty();
	}

	Conditions const & GetConditions() const {
		return m_conditions;
	}

private:
	Conditions m_conditions;
};

class CreateTableStatement : public ISQLStatement {
public:
	CreateTableStatement(std::string const & tableName, ColumnDescriptors const & columns)
		: ISQLStatement(SQLStatementType::CREATE_TABLE, tableName)
		, m_columns(columns)
	{}

	ColumnDescriptors const & GetColumns() const {
		return m_columns;
	}

private:
	ColumnDescriptors const m_columns; /// first -- name, second -- type
};

class CreateIndexStatement : public ISQLStatement {
public:
	CreateIndexStatement(std::string const & tableName, std::string const & indexName,
			std::vector<std::string> const & columns, bool isUnique = false)
		: ISQLStatement(SQLStatementType::CREATE_INDEX, tableName)
		, m_name(indexName)
		, m_columns(columns)
		, m_isUnique(isUnique)
	{}

	std::string const & GetName() const {
		return m_name;
	}

	std::vector<std::string> const & GetColumns() const {
		return m_columns;
	}

	bool IsUnique() const {
		return m_isUnique;
	}

private:
	std::string const m_name;
	std::vector<std::string> const m_columns;
	bool const m_isUnique;
};

class SelectStatement : public ISQLStatement, public WithConditions {
public:
	SelectStatement(std::string const & tableName, std::vector<std::string> const & fields,
			Conditions const & clause = Conditions())
		: ISQLStatement(SQLStatementType::SELECT, tableName)
		, WithConditions(clause)
		, m_fields(fields)
	{}

	std::vector<std::string> const & GetFields() const {
		return m_fields;
	}

private:
	std::vector<std::string> const m_fields;
};

class InsertStatement : public ISQLStatement {
public:
	InsertStatement(std::string const & tableName, std::vector<std::string> const & columns,
			Values const & values)
		: ISQLStatement(SQLStatementType::INSERT, tableName)
		, m_columns(columns)
		, m_values(values)
	{}
	InsertStatement(std::string const & tableName, Values const & values)
		: InsertStatement(tableName, std::vector<std::string>(), values)
	{}

	std::vector<std::string> const & GetColumns() const {
		return m_columns;
	}

	Values const & GetValues() const {
		return m_values;
	}

private:
	std::vector<std::string> const m_columns;
	Values const m_values;
};

class UpdateStatement : public ISQLStatement, public WithConditions {
public:
	UpdateStatement(std::string const & tableName, std::map<std::string, Value> const & colVals,
			Conditions const & conditions)
		: ISQLStatement(SQLStatementType::UPDATE, tableName)
		, WithConditions(conditions)
		, m_colVals(colVals)
	{}

	std::map<std::string, Value> const & GetColVals() const {
		return m_colVals;
	}

private:
	std::map<std::string, Value> m_colVals;
};

class DeleteStatement : public ISQLStatement, public WithConditions {
public:
	DeleteStatement(std::string const & tableName, Conditions const & clause)
		: ISQLStatement(SQLStatementType::DELETE, tableName)
		, WithConditions(clause)
	{}
};

#endif // SQLStatement_h
