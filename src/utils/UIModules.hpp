#ifndef DATAGRAPHTOOL_FORMS_UIMODULES_H__
#define DATAGRAPHTOOL_FORMS_UIMODULES_H__

#include <string>
#include <string_view>
#include <optional>
#include <unordered_map>
#include <map>

namespace DataGraph::utils
{

void companyComboBox(std::string_view name, uint32_t& companyId, std::string& companyPreview, std::string& companySearch);
void ownerComboBox(std::string_view name, uint32_t& currentOwnerId, std::string& comboBoxPreview, std::string& searchName);
void departmentComboBox(std::string_view name, std::unordered_map<std::string, uint32_t>& chosenDepartments);

// Could be replaced with templated one but current version with some code duplication is fine for now
void inputEmployeeCount(std::string_view name, std::optional<int>& employeeCount);
void inputDate(std::string_view name, std::optional<struct tm>& date);
bool inputFile(std::string_view name, std::string& loadedFile, std::string_view* errorMessage = nullptr);
void inputProjectName(std::string_view name, uint32_t& projectId, std::string& projectNamePreview, std::string& projectNameSearch);
void inputProjectVersion(std::string_view name,
	 uint32_t& projectId,
	 std::string& projectName,
	 uint32_t& projectVersionId,
	 std::string& projectVersionNamePreview,
	 std::string& projectVersionSearch);

void previousProjectVersionCombo(std::string_view name,
	 uint32_t& projectId,
	 std::string& projectNamePreview,
	 std::string& projectNameSearch,
	 uint32_t& projectVersionId,
	 std::string& projectVersionNamePreview,
	 std::string& projectVersionSearch,
	 std::map<int, std::string>& prevProjectVersionsList);

}  // namespace DataGraph::utils
#endif	// !DATAGRAPHTOOL_FORMS_UIMODULES_H__
