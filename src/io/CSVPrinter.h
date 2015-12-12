#ifndef CSVPrinter_h
#define CSVPrinter_h

#include <ostream>
#include <vector>

#include <common/Common.h>

class CSVPrinter {
public:
	CSVPrinter(std::ostream & stream);

	void PrintHeading(ColumnDescriptors const & descriptors);
	void PrintLine(Values const & values);

private:
	std::string GetTypeStr(ValueType type);

private:
	std::ostream & m_stream;
};

#endif // CSVPrinter_h
