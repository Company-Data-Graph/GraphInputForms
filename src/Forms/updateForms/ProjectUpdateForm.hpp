#ifndef DATAGRAPHTOOL_FORMS_UPDATEFORMS_PROJECTUPDATEFORM_H__
#define DATAGRAPHTOOL_FORMS_UPDATEFORMS_PROJECTUPDATEFORM_H__

#include <map>
#include <string_view>
#include <optional>
#include <vector>
#include <unordered_map>

#include <Forms/Form.hpp>

namespace DataGraph::Forms
{
class ProjectUpdate: public Form
{
public:
	int init() override final;
	int draw() override final;
	const char* name() const override final;
	void reset() final;

	std::string_view getStatusMessage() const override final;
	int getStatusCode() const override final;

private:
	void resetVersionOrderList();

	uint32_t m_errorCode = -1;
	std::string_view m_errorMessage;
	std::string_view m_warningMessage;

	uint32_t m_findProjectId = -1;
	std::string m_findProjectName;
	std::string m_findProjectNameSearch;

	uint32_t m_findProjectVersionId = -1;
	std::string m_findProjectVersion;
	std::string m_findProjectVersionSearch;

	uint32_t m_projectListId = -1;
	std::string m_projectListName;
	std::string m_projectListNameSearch;

	uint32_t m_projectListVersionId;
	std::string m_projectListVersion;
	std::string m_projectListVersionSearch;

	uint32_t m_parentProjectId = -1;
	std::string m_parentProjectName;
	std::string m_parentProjectNameSearch;

	std::string m_projectName;
	std::string m_projectNewVersion;
	std::string m_projectDesc;
	std::string m_pressReleaseURL;
	std::optional<std::string> m_iconPath;

	uint32_t m_companyId = -1;
	std::string m_companyName;
	std::string m_companySearchName;

	std::string m_projectNameSearch;
	std::string m_projectVersionSearch;
	std::map<int, std::string> m_previousVersionsList;

	std::optional<struct tm> m_date;

	std::string m_newIconPath;

	std::unordered_map<std::string, uint32_t> m_departmentsList;

	uint32_t m_projectVersionIndex = -1;
	uint32_t m_projectVersionOrderIndex = -1;

	bool m_projectVersionIndexChanged;
	std::vector<std::string> m_versionOrderList;

	int m_x = 0;
	int m_y = 0;
};
}  // namespace DataGraph::Forms
#endif
