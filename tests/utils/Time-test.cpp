#include <utils/Time.hpp>

#include <gtest/gtest.h>
#include <cstring>
#include <time.h>
#include <stdlib.h>

using namespace DataGraph;

TEST(strptimeTest, ValidInput)
{
	struct tm t = {0};
	char* res = utils::strptime("2023-02-17 18:15:30", "%Y-%m-%d %H:%M:%S", &t);
	EXPECT_STREQ(res, "");
	EXPECT_EQ(t.tm_year, 123);
	EXPECT_EQ(t.tm_mon, 1);
	EXPECT_EQ(t.tm_mday, 17);
	EXPECT_EQ(t.tm_hour, 18);
	EXPECT_EQ(t.tm_min, 15);
	EXPECT_EQ(t.tm_sec, 30);
}

TEST(strptimeTest, InvalidInput)
{
	struct tm t = {0};
	char* res = utils::strptime("1!7-02-2023 18:15:30", "%Y-%m-%d %H:%M:%S", &t);
	EXPECT_EQ(res, (char*)0);
}

TEST(strptimeTest, NoFormatString)
{
	const char* date = "2023-02-17 18:15:30";
	struct tm t = {0};
	char* res = utils::strptime("2023-02-17 18:15:30", "", &t);
	EXPECT_EQ(res, date);
}

TEST(strptimeTest, NoInputString)
{
	struct tm t = {0};
	const char* date = "";
	char* res = utils::strptime(date, "%Y-%m-%d %H:%M:%S", &t);
	EXPECT_EQ(res, (char*)0);
}

TEST(strptimeTest, NullPointer)
{
	const char* date = "2023-02-17 18:15:30";
	char* res = utils::strptime(date, "%Y-%m-%d %H:%M:%S", nullptr);
	EXPECT_EQ(res, date);
}
