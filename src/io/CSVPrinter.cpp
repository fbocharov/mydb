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

void CSVPrinter::PrintLine(std::vector<std::string> const & values) {
	for (size_t i = 0; i < values.size(); ++i) {
		m_stream << values[i];
		if (i + 1 != values.size())
			m_stream << ",";
	}
	m_stream << std::endl;
}

std::string CSVPrinter::GetTypeStr(FieldType type) {
	// TODO: cache return values.
	switch (type) {
		case FieldType::INT:
			return "int";
		case FieldType::DOUBLE:
			return "double";
		case FieldType::VARCHAR:
			return "varchar";
		default:
			return "unknown";
	}
}
