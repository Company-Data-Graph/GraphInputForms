#include "StringParse.hpp"

#include <corecrt_wtime.h>

#include <fmt/format.h>

namespace DataGraph::utils
{
std::string parseDepartments(const std::unordered_map<std::string, uint32_t>& chosenDepartments)
{
	std::string parseString;
	if (!chosenDepartments.empty())
	{
		for (auto& [_, id] : chosenDepartments)
		{
			parseString += std::to_string(id) + ',';
		}
		parseString.pop_back();
	}

	return parseString;
}
std::string parseDate(const std::optional<tm>& date)
{
	if (date.has_value())
	{
		auto& dt = date.value();
		return fmt::format("{0}-{1}-{2}", 1900 + dt.tm_year, dt.tm_mon + 1, dt.tm_mday);
	}

	return "Infinity";
}
std::string parseLastNode(const std::map<int, std::string>& prevProjVersions)
{
	std::string lastNodes;
	for (auto& [id, _] : prevProjVersions)
	{
		lastNodes += std::to_string(id) + ',';
	}

	if (!lastNodes.empty())
	{
		lastNodes.pop_back();
	}

	return lastNodes;
}

// A public domain algorithm published by the C/C++ Users Journal
bool WildcardMatch(const char* pat, const char* str)
{
	int i, star;

new_segment:

	star = 0;
	if (*pat == '*')
	{
		star = 1;
		do
		{
			pat++;
		} while (*pat == '*');
	}

test_match:

	for (i = 0; pat[i] && (pat[i] != '*'); i++)
	{
		if (str[i] != pat[i])
		{
			if (!str[i])
				return 0;
			if ((pat[i] == '?') && (str[i] != '.'))
				continue;
			if (!star)
				return 0;
			str++;
			goto test_match;
		}
	}
	if (pat[i] == '*')
	{
		str += i;
		pat += i;
		goto new_segment;
	}
	if (!str[i])
		return 1;
	if (i && pat[i - 1] == '*')
		return 1;
	if (!star)
		return 0;
	str++;
	goto test_match;
}
}  // namespace DataGraph::utils
