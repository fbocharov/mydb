#ifndef SQLStatement_h
#define SQLStatement_h

#include <vector>
#include <string>
#include <utility>

enum class SQLStatementType {
	CREATE,
	SELECT,
	INSERT
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


class CreateStatement : public ISQLStatement {
public:
	CreateStatement(std::string const & tableName, std::vector<std::pair<std::string, std::string>> const & columnTypes)
		: ISQLStatement(SQLStatementType::CREATE, tableName)
		, m_columnTypes(columnTypes)
	{}

	std::vector<std::pair<std::string, std::string>> const & GetColumnTypes() const {
		return m_columnTypes;
	}

private:
	std::vector<std::pair<std::string, std::string>> m_columnTypes; /// first -- name, second -- type
};


class SelectStatement : public ISQLStatement {
public:
	SelectStatement(std::string const & tableName, std::vector<std::string> const & fields)
		: ISQLStatement(SQLStatementType::SELECT, tableName)
		, m_fields(fields)
	{}

	std::vector<std::string> const & GetFields() const {
		return m_fields;
	}

private:
	std::vector<std::string> m_fields;
};


class InsertStatement : public ISQLStatement {
public:
	InsertStatement(std::string const & tableName, std::vector<std::string> const & values)
		: ISQLStatement(SQLStatementType::INSERT, tableName)
		, m_values(values)
	{}

	std::vector<std::string> const & GetValues() const {
		return m_values;
	}

private:
	std::vector<std::string> m_values;
};

#endif // SQLStatement_h
