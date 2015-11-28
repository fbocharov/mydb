#ifndef SQLParserException_h
#define SQLParserException_h

#include <stdexcept>
#include <string>

class SQLParserException : public std::runtime_error {
public:
	SQLParserException(std::string const & what)
		: std::runtime_error(what)
	{}
};

#endif // SQLParserException_h
