#ifndef Common_h
#define Common_h

#include <vector>
#include <string>
#include <cstdint>

enum class ValueType : std::uint8_t {
	UNKNOWN,
	INT,
	DOUBLE,
	VARCHAR
};

struct Value {
    Value()
    {
        type = ValueType::UNKNOWN;
        value = "";
    }
    Value(ValueType vt, std::string str)
    {
        type = vt, value = str;
    }
	ValueType type;
	std::string value;
};

typedef std::vector<Value> Values;


static size_t constexpr VARCHAR_MAX_LENGTH = 128; // should be less than uint8_t::max
static size_t constexpr COLUMN_NAME_LENGTH = 64;

struct ColumnDescriptor {
	ColumnDescriptor();
	ColumnDescriptor(char const * nm, ValueType tp, std::uint8_t sz);

	static ColumnDescriptor Deserialize(char const * data);
	void Serialize(char * data);

	char name[COLUMN_NAME_LENGTH];
	ValueType type;
	std::uint8_t size; /// in bytes

	static uint32_t constexpr DESCRIPTOR_SIZE =
		COLUMN_NAME_LENGTH + 2;
};

typedef std::vector<ColumnDescriptor> ColumnDescriptors;


#endif // Common_h
