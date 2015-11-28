#include "Record.h"
#include "DataPage.h"

Record::Record(DataPage & parentPage, uint16_t number, char const * data,
		std::map<std::string, ColumnDescriptor> const & descriptors)
	: m_parentPage(parentPage)
	, m_number(number)
	, m_descriptors(descriptors)
{
	uint16_t offset = 1; // TODO: replace by sizeof flags.
	for (auto const & nameDesc: descriptors) {
		uint8_t const size = nameDesc.second.size;
		m_columnValue[nameDesc.first] = std::string(data + offset, size);
		offset += size;
	}
}

Record::~Record() {
	m_parentPage.UpdateRecord(m_number, m_columnValue);
}
