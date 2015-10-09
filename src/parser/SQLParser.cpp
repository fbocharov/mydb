#include <iostream>
#include <boost/regex.hpp>

#include "SQLParser.h"
#include "SQLParserException.h"

SQLParser::SQLParser()
	: CREATE_TABLE_REGEX("CREATE\\s+TABLE\\s+[a-z]+\\((\\s*[a-z]+\\s+[a-z]+,){0,}\\);", boost::regex::icase)
	, SELECT_FROM_REGEX("SELECT\\s+\\*\\s+FROM\\s+[a-z]+;", boost::regex::icase)
	, INSERT_INTO_REGEX("INSERT\\s+INTO\\s+[a-z]+(\\((\\s*[a-z]+,){1,}\\))? VALUES \\((\\s*[a-z]+\\s*,\\s*){1,}\\);", boost::regex::icase)
{}

SQLParser & SQLParser::Instance() {
	static SQLParser parser;
	return parser;
}

std::unique_ptr<ISQLStatement> SQLParser::ParseStatement(std::string const & statement) {
	boost::smatch cmatch;
	if (boost::regex_match(statement, cmatch, CREATE_TABLE_REGEX)) {
		for(size_t i = 0; i < cmatch.size(); ++i) {
			std::cout << cmatch[i] << std::endl;
		}

		return ParseCreateTableStatement(cmatch);
	} else if (boost::regex_match(statement, cmatch, SELECT_FROM_REGEX))
		return ParseSelectStatement(statement);
	else if (boost::regex_match(statement, cmatch, INSERT_INTO_REGEX))
		return ParseInsertStatement(statement);

	assert(false && "SQLParser: failed to parse statement.");
	throw SQLParserException("SQLParser: failed to parse statement: " + statement);
}

std::unique_ptr<ISQLStatement> SQLParser::ParseCreateTableStatement(boost::smatch const & matches) {
	for (auto m : matches) {
		std::cout << m << std::endl;
	}

	return nullptr;
}

std::unique_ptr<ISQLStatement> SQLParser::ParseSelectStatement(std::string const & statement) {
	// TODO
	assert(false && "IMPLEMENT ME");
	return nullptr;
}

std::unique_ptr<ISQLStatement> SQLParser::ParseInsertStatement(std::string const & statement) {
	// TODO
	assert(false && "IMPLEMENT ME");
	return nullptr;
}
