#include <iostream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <utils/Log.h>

#include "SQLParser.h"
#include "SQLParserException.h"

SQLParser::SQLParser()
	: CREATE_TABLE_REGEX("CREATE\\s+TABLE\\s+([\\w_]+)\\(((\\s*[\\w]+\\s+[\\w]+,|\\s*[\\w]+\\s+[\\w]+\\s*)+)\\);", boost::regex::icase)
	, SELECT_FROM_REGEX("SELECT\\s+\\*\\s+FROM\\s+([\\w_]+);", boost::regex::icase)
	, INSERT_INTO_REGEX("INSERT\\s+INTO\\s+([\\w]+)\\s+VALUES\\s*\\(((\\s*[\\w]+\\s*,|\\s*[\\w]+\\s*)+)\\);", boost::regex::icase)
{}

SQLParser & SQLParser::Instance() {
	static SQLParser parser;
	return parser;
}

std::unique_ptr<ISQLStatement> SQLParser::ParseStatement(std::string const & statement) {
	Log(LogType::Debug) << "Parsing statement: " << statement << std::endl;

	boost::smatch what;
	if (boost::regex_match(statement, what, CREATE_TABLE_REGEX)) {
		return ParseCreateTableStatement(what);
	} else if (boost::regex_match(statement, what, SELECT_FROM_REGEX))
		return ParseSelectStatement(what);
	else if (boost::regex_match(statement, what, INSERT_INTO_REGEX))
		return ParseInsertStatement(what);

	Log(LogType::Info) << "Failed to parse statement."<< std::endl;
	assert(false && "SQLParser: failed to parse statement.");
	throw SQLParserException("SQLParser: failed to parse statement: " + statement);
}

std::unique_ptr<ISQLStatement> SQLParser::ParseCreateTableStatement(boost::smatch const & what) {
	std::string const tableName(what[1]);

	std::vector<std::pair<std::string, std::string>> columnTypes;
	std::vector<std::string> strings;
	std::vector<std::string> columnType;
	std::string str(what[2]);
	boost::split(strings, str, boost::is_any_of(","));
	for (size_t i = 0; i < strings.size(); ++i) {
		std::string const token = boost::trim_copy(strings[i]);
		boost::split(columnType, token, boost::is_any_of(" "));
		columnTypes.emplace_back(columnType[0], columnType[1]);
		columnType.clear();
	}

	return std::make_unique<CreateStatement>(tableName, columnTypes);
}

std::unique_ptr<ISQLStatement> SQLParser::ParseSelectStatement(boost::smatch const & what) {
	return std::make_unique<SelectStatement>(std::string(what[1]), std::vector<std::string>());
}

std::unique_ptr<ISQLStatement> SQLParser::ParseInsertStatement(boost::smatch const & what) {
	std::string const tableName(what[1]);

	std::vector<std::string> values;
	std::vector<std::string> strings;
	std::string str(what[2]);
	boost::split(strings, str, boost::is_any_of(","));
	for (size_t i = 0; i < strings.size(); ++i) {
		boost::trim(strings[i]);
		values.emplace_back(strings[i]);
	}

	return std::make_unique<InsertStatement>(tableName, values);
}
