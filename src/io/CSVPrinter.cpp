#include "CSVPrinter.h"

CSVPrinter::CSVPrinter(std::ostream & stream)
	: m_stream(stream)
{}

void CSVPrinter::PrintHeading(ColumnDescriptors const & descriptors) {
	for (auto const & desc: descriptors)
		m_stream << desc.name << "(" << GetTypeStr(desc.type) << "),";
	m_stream << std::endl;
}

void CSVPrinter::PrintLine(std::vector<std::string> const & values) {
	for (auto const & v: values)
		m_stream << v << ",";
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
