#define _CRT_SECURE_NO_WARNINGS // for vs

#include <algorithm>

#include <utils/Log.h>

#include "SQLParser.h"
#include "SQLParserException.h"
#include "SQLParserInternals.hpp"
#include "SQLScannerInternals.hpp"

/// Used by generated code to return parsed statement.
ISQLStatement * parsedStatement;

SQLParser & SQLParser::Instance() {
	static SQLParser parser;
	return parser;
}

std::unique_ptr<ISQLStatement> SQLParser::ParseStatement(std::string const & statement) {
	std::string lowerStmt;
	std::transform(statement.begin(), statement.end(), std::back_inserter(lowerStmt), tolower);
	auto result = yy_scan_string(lowerStmt.c_str());
	if (yyparse()) {
		yy_delete_buffer(result);
		throw SQLParserException("Unknown command!");
	}
	yy_delete_buffer(result);
	return std::unique_ptr<ISQLStatement>(parsedStatement);
}
