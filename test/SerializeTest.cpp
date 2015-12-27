#include <gtest/gtest.h>

#include <cstring>

#include <common/Common.h>

TEST(SerializeTest, ColumnDescriptorSerializeTest) {
	char const * name = "users";
	char buf[ColumnDescriptor::DESCRIPTOR_SIZE];

	ColumnDescriptor before;
	before.size = static_cast<uint8_t>(strlen(name));
	memcpy(before.name, name, before.size);
	before.type = ValueType::VARCHAR;
	before.Serialize(buf);

	ColumnDescriptor after = ColumnDescriptor::Deserialize(buf);
	ASSERT_EQ(0, strcmp(before.name, after.name));
	ASSERT_EQ(before.type, after.type);
	ASSERT_EQ(before.size, after.size);

	memset(buf, 0, ColumnDescriptor::DESCRIPTOR_SIZE);
	before.size = static_cast<uint8_t>(strlen(name));
	memcpy(before.name, name, before.size);
	before.type = ValueType::VARCHAR;
	before.Serialize(buf);

	after = ColumnDescriptor::Deserialize(buf);
	ASSERT_EQ(0, strcmp(before.name, after.name));
	ASSERT_EQ(before.type, after.type);
	ASSERT_EQ(before.size, after.size);
}
