#include "Record.h"
#include "DataPage.h"

Record::Record(DataPage & parentPage, uint16_t number, char const * data,
		ColumnDescriptors const & descriptors)
	: m_parentPage(parentPage)
	, m_number(number)
	, m_isDirty(false)
{
	uint16_t offset = 1; // TODO: replace by sizeof flags.
	for (auto const & desc: descriptors) {
		uint8_t const size = desc.size;
		m_values.push_back(std::string(data + offset, size));
		m_columnValue[desc.name] = &m_values.back();
		offset += size;
	}
}

std::vector<std::string> const & Record::GetValues() const {
	return m_values;
}


//std::string & Record::operator[] (std::string const & key) {
//	auto it = m_columnValue.find(key);
//	if (m_columnValue.end() == it)
//		throw std::runtime_error("Record don't contain key " + key);
//	m_isDirty = true;
//	return *it->second;
//}

//std::string const & Record::operator[] (std::string const & key) const {
//	auto it = m_columnValue.find(key);
//	if (m_columnValue.end() == it)
//		throw std::runtime_error("Record don't contain key " + key);
//	return *it->second;
//}

Record::~Record() {
//	if (m_isDirty)
//		m_parentPage.UpdateRecord(m_number, m_columnValue);
}
