#ifndef DATAGRAPHTOOL_FORMS_STRINGPARSE_H__
#define DATAGRAPHTOOL_FORMS_STRINGPARSE_H__

#include <string>
#include <map>
#include <unordered_map>
#include <optional>

namespace DataGraph::utils
{
std::string parseDepartments(const std::unordered_map<std::string, uint32_t>& chosenDepartments);
std::string parseDate(const std::optional<struct tm>& date);
std::string parseLastNode(const std::map<int, std::string>& prevProjVersions);
}  // namespace DataGraph::utils
#endif	// !DATAGRAPHTOOL_FORMS_UIMODULES_H__
