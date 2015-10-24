#include <gtest/gtest.h>

#include <utils/Utils.h>

TEST(UtilsTest, BytesToNumTest) {
	char buf[sizeof(size_t)] = {0};
	size_t num = -1;
	BytesToNumber(buf, num);
	ASSERT_EQ(0, num);
}

TEST(UtilsTest, NumToBytesTest) {
	size_t num = 0;
	char buf[sizeof(size_t)];
	NumberToBytes(num, buf);
	for (size_t i = 0; i < sizeof(size_t); ++i)
		ASSERT_EQ(0, buf[i]);
}
