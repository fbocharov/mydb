#ifndef Common_h
#define Common_h

#include <cstdint>
#include <cstddef>
#include <vector>

enum class FieldType : std::uint8_t {
	INT,
	FLOAT,
	VARCHAR
};

static size_t constexpr VARCHAR_MAX_LENGTH = 128; // should be less than uint8_t::max
static size_t constexpr COLUMN_NAME_LENGTH = 64;

struct ColumnDescriptor {
	static ColumnDescriptor Deserialize(char const * data);
	void Serialize(char * data);

	char name[COLUMN_NAME_LENGTH];
	FieldType type;
	std::uint8_t size; /// in bytes

	static uint32_t constexpr DESCRIPTOR_SIZE =
		COLUMN_NAME_LENGTH + 2;
};

typedef std::vector<ColumnDescriptor> ColumnDescriptors;

#endif // Common_h
