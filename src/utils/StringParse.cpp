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
		parseString.back() = '\0';
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
}  // namespace DataGraph::utils
