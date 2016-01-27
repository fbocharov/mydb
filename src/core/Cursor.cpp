#include "Cursor.h"
#include <cassert>

// Stub
Cursor::Cursor(ColumnDescriptors const& descriptors)
	: m_descriptors(descriptors)
	, m_fields(std::vector<std::string>()) 
{
}

Value Cursor::Get(std::string const& column) const {
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

Values Cursor::GetAll() const {
	char const * record = GetCurrentRecord();
	Values values;
	++record; // skip delete bit
	for (auto const & descriptor : m_descriptors) {
		values.emplace_back(descriptor.type, std::string(record, descriptor.size));
		record += descriptor.size;
	}

	return values;
}

bool Cursor::Next() {
	if (!HasNext())
		return false;

	do
		GoToNextRecord();
	while (HasNext() && !SatisfiesAll());

	return HasNext() && SatisfiesAll();
}

bool Cursor::SatisfiesAll() const
{
	// NOTE: here we use fact that all conditions are connected with AND.
	// If you wanna add OR you should also change logic here.
	char const * record = GetCurrentRecord();
	if (*record)
		return false;
	for (auto const & condition : m_conditions) {
		Value v = Get(condition.GetColumn());
		if (!condition.Satisfies(v))
			return false;
	}

	return true;
}
