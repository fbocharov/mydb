#include <iostream>

#include <sql/SQLParser.h>
#include <sql/SQLParserException.h>
#include <io/InputReader.h>
#include <io/CSVPrinter.h>
#include <core/MyDB.h>
#include <utils/Log.h>

void PrintUsage(std::string const & appName) {
	std::cout << "Usage: " << appName << " <db_file>" << std::endl;
}

void ExecuteSelect(MyDB & db, std::unique_ptr<ISQLStatement> const & statement, CSVPrinter & printer) {
	auto cursor = db.ExecuteQuery(statement);
	auto tableDescription = db.GetTableDescription(statement->GetTableName());

	auto const & select = static_cast<SelectStatement const &>(*statement);
	auto fields = select.GetFields();
	if (!fields.empty()) {
		ColumnDescriptors descriptors;

		for (auto const & field : SplitQualifiedVectorUnzip(fields).second)
			for (auto const & desc : tableDescription)
				if (field == SplitQualified(desc.name).second)
					descriptors.push_back(desc);

		printer.PrintHeading(descriptors);
	} else
		printer.PrintHeading(tableDescription);

	while (cursor->Next())
		printer.PrintLine(cursor->GetAll());
}

void ExecuteStatement(MyDB & db, std::unique_ptr<ISQLStatement> const & statement, CSVPrinter & printer) {
	switch (statement->GetType()) {
		case SQLStatementType::CREATE_TABLE:
		case SQLStatementType::CREATE_INDEX: {
			if (db.ExecuteCreate(statement))
				std::cout << "OK" << std::endl;
			break;
		}
		case SQLStatementType::INSERT:
		case SQLStatementType::UPDATE:
		case SQLStatementType::DELETE: {
			size_t rows = db.ExecuteUpdate(statement);
			std::cout << "OK " << rows << std::endl;
			break;
		}
		case SQLStatementType::SELECT_JOIN:
		case SQLStatementType::SELECT: {
			ExecuteSelect(db, statement, printer);
			break;
		}
		default:
			std::cout << "Unknown sql statement." << std::endl;
	}
}

int main(int argc, char * argv[]) {
	if (2 != argc) {
		PrintUsage(argv[0]);
		return 1;
	}
	std::ofstream log("mydb.log");
	Log::SetStream(LogType::Info, log);
	Log::SetStream(LogType::Debug, log);
	Log::SetStream(LogType::Error, log);

	std::string userInput;
	InputReader reader(std::cin);
	CSVPrinter printer(std::cout);
	Log(LogType::Info) << "Start listening to unput." << std::endl;
	try {
		MyDB db(argv[1]);
		std::cout << "Hello from MyDB!" << std::endl;
		while (true) {
			std::cout << "> ";
			userInput = reader.Read();
			try {
				auto statement = SQLParser::Instance().ParseStatement(userInput);
				if (!statement) { // input == quit/exit
					std::cout << "Goodbye!" << std::endl;
					break;
				}
				ExecuteStatement(db, statement, printer);
			} catch (SQLParserException const & e) {
				std::cerr << "ERROR: " << e.what() << std::endl;
				Log(LogType::Error) << "Exception while parsing " << userInput << ": " << e.what() << std::endl;
			} catch(std::runtime_error const & e) {
				std::cerr << "ERROR: " << e.what() << std::endl;
				Log(LogType::Error) << "Exception in main: " << e.what() << std::endl;
			}
		}
	} catch (std::exception const & e) {
		std::cerr << "FATAL ERROR: " << e.what() << std::endl;
		Log(LogType::Error) << "std::exception: " << e.what() << std::endl;
		return 1;
	}

//	try {
//		MyDB db("InsertTest.mdb");
//		ColumnDescriptors descriptors;
//		descriptors.emplace_back(ColumnDescriptor("id", ValueType::INT, 4));
//		descriptors.emplace_back(ColumnDescriptor("name", ValueType::VARCHAR, 30));
//		std::unique_ptr<ISQLStatement> create = std::make_unique<CreateTableStatement>("test", descriptors);
//		db.ExecuteCreate(create);

//		std::vector<std::string> columns;
//		columns.push_back("id");
//		columns.push_back("name");
//		Values values(2);
//		for (size_t i = 0; i < 100000; ++i) {
//			values[0] = Value{ValueType::INT, i};
//			values[1] = Value{ValueType::VARCHAR, "dummy " + std::to_string(i)};
//			std::unique_ptr<ISQLStatement> insert = std::make_unique<InsertStatement>("test", columns, values);
//			db.ExecuteUpdate(insert);
//			if (0 == i % 10000)
//				std::cout << "Inserted " << i << std::endl;
//		}
//	} catch (std::exception const & e) {
//		std::cerr << e.what() << std::endl;
//		return 1;
//	}

	return 0;
}
