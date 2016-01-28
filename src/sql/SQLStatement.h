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
	SELECT_JOIN,
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

class WithFields {
public:
	WithFields(std::vector<std::string> const & fields)
		: m_fields(fields)
	{}
	virtual ~WithFields() = default;

	std::vector<std::string> const & GetFields() const {
		return m_fields;
	}

private:
	std::vector<std::string> m_fields;
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

class CreateIndexStatement : public ISQLStatement, public WithFields {
public:
	CreateIndexStatement(std::string const & tableName, std::string const & indexName,
			std::vector<std::string> const & fields, bool isUnique = false)
		: ISQLStatement(SQLStatementType::CREATE_INDEX, tableName)
		, WithFields(fields)
		, m_name(indexName)
		, m_isUnique(isUnique)
	{}

	std::string const & GetName() const {
		return m_name;
	}

	bool IsUnique() const {
		return m_isUnique;
	}

private:
	std::string const m_name;
	bool const m_isUnique;
};

class SelectStatement : public ISQLStatement, public WithConditions, public WithFields {
public:
	SelectStatement(std::string const & tableName, std::vector<std::string> const & fields,
			Conditions const & clause = Conditions())
		: ISQLStatement(SQLStatementType::SELECT, tableName)
		, WithConditions(clause)
		, WithFields(fields)
	{}
};

class JoinStatement : public ISQLStatement, public WithConditions, public WithFields {
public:
	JoinStatement(std::string const & leftTable, std::string const & rightTable,
			std::vector<std::string> const & fields, std::string const & leftField,
			std::string const & rightField, Conditions const & clause = Conditions())
		: ISQLStatement(SQLStatementType::SELECT_JOIN, leftTable)
		, WithConditions(clause)
		, WithFields(fields)
		, m_rightTable(rightTable)
		, m_leftField(leftField)
		, m_rightField(rightField)
	{}

	std::string const & GetLeftTableName() const {
		return GetTableName();
	}

	std::string const & GetRightTableName() const {
		return m_rightTable;
	}

	std::pair<std::string, std::string> GetJoinFields() const {
		return std::pair<std::string, std::string>(m_leftField, m_rightField);
	}

private:
	std::string const m_rightTable;
	std::string const m_leftField;
	std::string const m_rightField;
};

class InsertStatement : public ISQLStatement, public WithFields {
public:
	InsertStatement(std::string const & tableName, std::vector<std::string> const & columns,
			Values const & values)
		: ISQLStatement(SQLStatementType::INSERT, tableName)
		, WithFields(columns)
		, m_values(values)
	{}

	InsertStatement(std::string const & tableName, Values const & values)
		: InsertStatement(tableName, std::vector<std::string>(), values)
	{}

	Values const & GetValues() const {
		return m_values;
	}

private:
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
