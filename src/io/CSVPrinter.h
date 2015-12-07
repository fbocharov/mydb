#ifndef CSVPrinter_h
#define CSVPrinter_h

#include <ostream>
#include <vector>

#include <common/Common.h>

class CSVPrinter {
public:
	CSVPrinter(std::ostream & stream);

	void PrintHeading(ColumnDescriptors const & descriptors);
	void PrintLine(std::vector<std::string> const & values);

private:
	std::string GetTypeStr(FieldType type);

private:
	std::ostream & m_stream;
};

#endif // CSVPrinter_h
