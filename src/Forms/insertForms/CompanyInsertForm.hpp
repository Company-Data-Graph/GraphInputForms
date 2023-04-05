#ifndef DATAGRAPHTOOL_FORMS_INSERTFORMS_COMPANYINSERT_H__
#define DATAGRAPHTOOL_FORMS_INSERTFORMS_COMPANYINSERT_H__

#include <string_view>
#include <optional>
#include <unordered_map>

#include <Forms/Form.hpp>

namespace DataGraph::Forms
{
class CompanyInsert: public Form
{
public:
	int init() final;
	int draw() final;
	const char* name() const final;
	void reset() final;

	std::string_view getStatusMessage() const override final;
	int getStatusCode() const override final;

private:
	uint32_t m_errorCode = -1;
	std::string_view m_errorMessage;
	std::string_view m_warningMessage;

	std::string m_companyName;

	uint32_t m_ownerId = -1;
	std::string m_ownerSearch;
	std::optional<int> m_employeeCount;
	std::string m_companyDesc;
	std::string m_companyAddr;

	std::string_view m_departmentPreview;
	std::unordered_map<std::string, uint32_t> m_chosenDepartments;

	std::optional<struct tm> m_date;

	std::string m_filePath;

	int m_x = 0;
	int m_y = 0;
};
}  // namespace DataGraph::Forms
#endif
