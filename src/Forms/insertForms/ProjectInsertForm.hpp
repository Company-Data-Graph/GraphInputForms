#ifndef DATAGRAPHTOOL_FORMS_INSERTFORMS_PROJECTINSERTFORM_H__
#define DATAGRAPHTOOL_FORMS_INSERTFORMS_PROJECTINSERTFORM_H__

#include <string_view>
#include <optional>
#include <unordered_map>
#include <map>

#include <Forms/Form.hpp>

namespace DataGraph::Forms
{
class ProjectInsert: public Form
{
public:
	int init() override final;
	int draw() override final;
	const char* name() const override final;
	void reset() final;

private:
	uint32_t m_errorCode;
	std::string_view m_errorMessage;

	std::string_view m_warningMessage;

	uint32_t m_projectId = -1;
	std::string m_projectName;
	std::string m_projectNameSearch;

	std::string m_projectNewName;

	uint32_t m_nodeId = -1;
	std::string m_projectVersion;
	std::string m_projectVersionSearch;

	std::optional<struct tm> m_date;

	uint32_t m_companyId = -1;
	std::string m_companyPreview;
	std::string m_companySearch;

	std::map<int, std::string> m_previousVersionsList;

	std::string m_projectListName;
	std::string m_projectListNameSearch;
	std::string m_projectListVersion;
	std::string m_projectListVersionSearch;

	std::string m_projectDesc;

	std::unordered_map<std::string, uint32_t> m_departmentsList;

	std::string m_filePath;

	std::string m_pressReleaseURL;

	int m_x = 0;
	int m_y = 0;
};
}  // namespace DataGraph::Forms
#endif
