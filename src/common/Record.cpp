#include <stdexcept>

#include "Record.h"

Record::Record(char const * data, ColumnDescriptors const & descriptors)
	: m_descriptors(descriptors)
{
	m_isDeleted = *data;
	size_t offset = 1; // TODO: replace by sizeof flags.
	for (auto const & desc: descriptors) {
		m_values.push_back(Value{desc.type, std::string(data + offset)});
		offset += desc.size;
	}
}

Values const & Record::GetValues() const {
	return m_values;
}

Value const & Record::operator[] (std::string const & column) const {
	for (size_t i = 0; i < m_descriptors.size(); ++i)
		if (column == m_descriptors[i].name)
			return m_values[i];
	throw std::runtime_error("Record doesn't contain field \"" + column + "\".");
}

bool Record::IsDeleted() const {
	return m_isDeleted;
}
