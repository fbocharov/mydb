#ifndef SQLParser_h
#define SQLParser_h

#include <string>
#include <memory>

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
	SQLParser() = default;
	~SQLParser() = default;
};

#endif // SQLParser_h
