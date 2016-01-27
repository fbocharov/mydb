#include "CursorImpl.h"
#include <stdexcept>
#include <cassert>

CursorImpl::CursorImpl(ColumnDescriptors const& descriptors)
	: m_descriptors(descriptors)
{}

bool CursorImpl::Next() {
	while (HasNext()) {
		GoToNextRecord();
		if (*GetCurrentRecord() == 0)
			return true;
	}

	return false;
}

Value CursorImpl::Get(std::string const& column) const {
	char const * record = GetCurrentRecord();
	assert(!*record); // checking for delete bit

	++record; // skip delete bit
	for (auto const & descriptor : m_descriptors) {
		if (descriptor.name == column)
			return{ descriptor.type, std::string(record, descriptor.size) };
		record += descriptor.size;
	}

	throw std::runtime_error("Record doesn't contain field \"" + column + "\".");
}

Values CursorImpl::GetAll() const {
	char const * record = GetCurrentRecord();
	Values values;
	++record; // skip delete bit
	for (auto const & descriptor : m_descriptors) {
		values.emplace_back(descriptor.type, std::string(record, descriptor.size));
		record += descriptor.size;
	}

	return values;
}
