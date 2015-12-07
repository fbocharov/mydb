#include <cstring>

#include <utils/Utils.h>

#include "Common.h"

ColumnDescriptor::ColumnDescriptor(char const * nm, FieldType tp)
	: type(tp)
	, size(strlen(nm))
{
	strncpy(name, nm, size);
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
