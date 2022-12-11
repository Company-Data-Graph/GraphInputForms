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

private:
	std::string_view m_errorMessage;

	int m_errorCode;
	std::string m_companySearch;
};
}  // namespace DataGraph::Forms
#endif
