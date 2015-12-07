#include <cstring>

#include <utils/Utils.h>

#include "Common.h"

ColumnDescriptor::ColumnDescriptor(char const * nm, FieldType tp, uint8_t sz)
	: type(tp)
	, size(sz)
{
	size_t const nmSize = strlen(nm);
	size_t const nameSize = nmSize + 1 >= COLUMN_NAME_LENGTH ? COLUMN_NAME_LENGTH : nmSize;
	strncpy(name, nm, nameSize);
	name[nameSize] = '\0';
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

void ColumnDescriptor::Serialize(char * data) {
	memcpy(data, name, COLUMN_NAME_LENGTH);
	data += COLUMN_NAME_LENGTH;
	NumberToBytes(type, data);
	data += 1;
	NumberToBytes(size, data);
}
