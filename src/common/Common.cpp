#include <cstring>
#include <string>
#include <stdexcept>

#include <utils/Utils.h>

#include "Common.h"
#include <algorithm>

ColumnDescriptor::ColumnDescriptor()
	: name{}
	, type(ValueType::UNKNOWN)
	, size(0)
{}

ColumnDescriptor::ColumnDescriptor(char const * nm, ValueType tp, uint8_t sz)
	: name{}
	, type(tp)
	, size(sz)
{
	size_t const nmSize = strlen(nm);
	size_t const nameSize = nmSize >= COLUMN_NAME_LENGTH - 1 ? COLUMN_NAME_LENGTH - 1 : nmSize;
	strncpy(name, nm, nameSize);
}

ColumnDescriptor ColumnDescriptor::Deserialize(char const * data) {
	ColumnDescriptor descriptor;

	memcpy(descriptor.name, data, COLUMN_NAME_LENGTH);
	data += COLUMN_NAME_LENGTH;
	BytesToNumber(data, descriptor.type);
	data += 1;
	BytesToNumber(data, descriptor.size);

	return descriptor;
}

void ColumnDescriptor::Serialize(char * data) const {
	memcpy(data, name, COLUMN_NAME_LENGTH);
	data += COLUMN_NAME_LENGTH;
	NumberToBytes(type, data);
	data += 1;
	NumberToBytes(size, data);
}

ColumnDescriptor GetDescriptorByName(ColumnDescriptors descriptors, std::string const& name)
{
	auto const & searchResult = std::find_if(descriptors.begin(), descriptors.end(),
		[name](const ColumnDescriptor & descriptor)
		-> bool { return name == descriptor.name; });

	if(searchResult == descriptors.end())
		throw std::runtime_error("Descriptor with name \"" + name + "\" not found.");
	
	return *searchResult;
}
