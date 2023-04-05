#ifndef DATAGRAPHTOOL_FORMS_DELETEFORMS_COMPANYDELETEFORM_H__
#define DATAGRAPHTOOL_FORMS_DELETEFORMS_COMPANYDELETEFORM_H__

#include <string_view>

#include <Forms/Form.hpp>

namespace DataGraph::Forms
{
class CompanyDelete: public Form
{
public:
	int init() final;
	int draw() final;
	const char* name() const final;
	void reset() final;

	std::string_view getStatusMessage() const override final;
	int getStatusCode() const override final;

private:
	int m_errorCode = -1;
	std::string_view m_errorMessage;

	uint32_t m_companyId = -1;
	std::string m_companyNamePreview;
	std::string m_companySearch;
};
}  // namespace DataGraph::Forms
#endif
