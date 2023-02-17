#include <gtest/gtest.h>
#include <unordered_map>
#include <string>
#include <optional>
#include <ctime>
#include <fmt/format.h>

#include <utils/StringParse.hpp>

using namespace DataGraph;

TEST(parseDepartmentsTest, EmptyDepartmentsTest)
{
	std::unordered_map<std::string, uint32_t> chosenDepartments;
	std::string expectedOutput = "";
	EXPECT_EQ(utils::parseDepartments(chosenDepartments), expectedOutput);
}

TEST(parseDepartmentsTest, NonEmptyDepartmentsTest)
{
	std::unordered_map<std::string, uint32_t> chosenDepartments = {{"dept1", 1}, {"dept2", 2}, {"dept3", 3}};
	std::string expectedOutput = "1,2,3";
	EXPECT_EQ(utils::parseDepartments(chosenDepartments), expectedOutput);
}

TEST(parseDateTest, EmptyDateTest)
{
	std::optional<tm> date = std::nullopt;
	std::string expectedOutput = "Infinity";
	EXPECT_EQ(utils::parseDate(date), expectedOutput);
}

TEST(parseDateTest, NonEmptyDateTest)
{
	std::optional<tm> date;
	std::time_t t = std::time(nullptr);
	date = *std::localtime(&t);
	std::string expectedOutput = fmt::format("{0}-{1}-{2}", 1900 + date->tm_year, date->tm_mon + 1, date->tm_mday);
	EXPECT_EQ(utils::parseDate(date), expectedOutput);
}

TEST(parseLastNodeTest, EmptyPrevProjVersionsTest)
{
	std::map<int, std::string> prevProjVersions;
	std::string expectedOutput = "";
	EXPECT_EQ(utils::parseLastNode(prevProjVersions), expectedOutput);
}

TEST(parseLastNodeTest, NonEmptyPrevProjVersionsTest)
{
	std::map<int, std::string> prevProjVersions = {{1, "version1"}, {2, "version2"}, {3, "version3"}};
	std::string expectedOutput = "1,2,3";
	EXPECT_EQ(utils::parseLastNode(prevProjVersions), expectedOutput);
}
