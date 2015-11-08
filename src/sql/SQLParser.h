#ifndef SQLParser_h
#define SQLParser_h

#include <string>
#include <memory>
#include <boost/regex.hpp>

#include "SQLStatement.h"

class SQLParser {
public:
	SQLParser(SQLParser const &) = delete;
	SQLParser(SQLParser &&) = delete;
	SQLParser & operator=(SQLParser const &) = delete;
	SQLParser & operator=(SQLParser &&) = delete;

	static SQLParser & Instance();

	std::unique_ptr<ISQLStatement> ParseStatement(std::string const & statement);

private:
	SQLParser();
	~SQLParser() = default;

	std::unique_ptr<ISQLStatement> ParseCreateTableStatement(boost::smatch const & what);
	std::unique_ptr<ISQLStatement> ParseSelectStatement(boost::smatch const & what);
	std::unique_ptr<ISQLStatement> ParseInsertStatement(boost::smatch const & what);

private:
	boost::regex const CREATE_TABLE_REGEX;
	boost::regex const SELECT_FROM_REGEX;
	boost::regex const INSERT_INTO_REGEX;
};

#endif // SQLParser_h