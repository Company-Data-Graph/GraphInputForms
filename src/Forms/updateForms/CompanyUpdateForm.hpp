#ifndef DATAGRAPHTOOL_FORMS_UPDATEFORMS_COMPANYUPDATEFORM_H__
#define DATAGRAPHTOOL_FORMS_UPDATEFORMS_COMPANYUPDATEFORM_H__

#include <optional>
#include <string_view>
#include <unordered_map>

#include <Forms/Form.hpp>

namespace DataGraph::Forms
{
class CompanyUpdate: public Form
{
public:
	int init() final;
	int draw() final;
	const char* name() const final;
	void reset() final;

	std::string_view getStatusMessage() const override final;
	int getStatusCode() const override final;

private:
	uint32_t m_companyId = -1;
	std::string m_companyNamePreview;
	std::string m_companySearch;

	uint32_t m_ownerId = -1;
	std::string m_ownerPreview;
	std::string m_ownerSearch;

	std::string m_companyName;
	std::string m_companyDesc;
	std::string m_companyAddr;
	std::optional<int> m_employees;
	std::optional<struct tm> m_date;

	std::unordered_map<std::string, uint32_t> m_chosenDepartments;

	std::string m_filePath;

	uint32_t m_errorCode = -1;
	std::string_view m_errorMessage;
	std::string_view m_warningMessage;

	int m_x = 0;
	int m_y = 0;
};
}  // namespace DataGraph::Forms
#endif
