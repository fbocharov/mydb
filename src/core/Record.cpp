#include "Record.h"
#include "DataPage.h"

Record::Record(DataPage & parentPage, uint16_t number, char const * data,
		std::map<std::string, ColumnDescriptor> const & descriptors)
	: m_parentPage(parentPage)
	, m_number(number)
	, m_descriptors(descriptors)
	, m_isDirty(false)
{
	uint16_t offset = 1; // TODO: replace by sizeof flags.
	for (auto const & nameDesc: descriptors) {
		uint8_t const size = nameDesc.second.size;
		m_columnValue[nameDesc.first] = std::string(data + offset, size);
		offset += size;
	}
}

std::string & Record::operator[] (std::string const & key) {
	auto it = m_columnValue.find(key);
	if (m_columnValue.end() == it)
		throw std::runtime_error("Record don't contain key " + key);
	m_isDirty = true;
	return it->second;
}

std::string const & Record::operator[] (std::string const & key) const {
	auto it = m_columnValue.find(key);
	if (m_columnValue.end() == it)
		throw std::runtime_error("Record don't contain key " + key);
	return it->second;
}

Record::~Record() {
	if (m_isDirty)
		m_parentPage.UpdateRecord(m_number, m_columnValue);
}
