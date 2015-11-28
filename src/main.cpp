#include <iostream>

#include <sql/SQLParser.h>
#include <sql/SQLParserException.h>
#include <utils/Log.h>

int main(int argc, char * argv[]) {
	Log::SetStream(LogType::Debug, std::cout);
	Log::SetStream(LogType::Error, std::cerr);

	Log(LogType::Info) << "Start listening to unput." << std::endl;

	std::string userInput;
	std::cout << "Hello from MyDB!" << std::endl;
	while (true) {
		std::cout << "> ";
		std::getline(std::cin, userInput);
		try {
			auto statement = SQLParser::Instance().ParseStatement(userInput);
		} catch (SQLParserException const & e) {
			std::cerr << "ERROR: " << e.what() << std::endl;
			Log(LogType::Error) << "Exception while parsing " << userInput << ": " << e.what() << std::endl;
		}
	}

	return 0;
}
