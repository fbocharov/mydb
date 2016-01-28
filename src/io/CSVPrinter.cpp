#include <sstream>
#include "CSVPrinter.h"

CSVPrinter::CSVPrinter(std::ostream & stream)
	: m_stream(stream)
{}

void CSVPrinter::PrintHeading(ColumnDescriptors const & descriptors) {
	for (size_t i = 0; i < descriptors.size(); ++i) {
		auto const & desc = descriptors[i];
		m_stream << desc.name << "(" << GetTypeStr(desc.type) << ")";
		if (i + 1 != descriptors.size())
			m_stream << ",";
	}
	m_stream << std::endl;
}

void CSVPrinter::PrintLine(Values const & values) {
	for (size_t i = 0; i < values.size(); ++i) {
		m_stream << values[i].ToString();
		if (i + 1 != values.size())
			m_stream << ",";
	}
	m_stream << std::endl;
}

std::string CSVPrinter::GetTypeStr(ValueType type) {
	// TODO: cache return values.
	switch (type) {
		case ValueType::INT:
			return "int";
		case ValueType::DOUBLE:
			return "double";
		case ValueType::VARCHAR:
			return "varchar";
		default:
			return "unknown";
	}
}
