#ifndef DATAGRAPHTOOL_FORMS_INSERTFORMS_COMPANYINSERT_H__
#define DATAGRAPHTOOL_FORMS_INSERTFORMS_COMPANYINSERT_H__

#include <string_view>

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

private:
	uint32_t m_errorCode;
	std::string_view m_errorMessage;

	std::string m_companyName;
	std::string m_ownerSearch;
	std::string m_companyDesc;
	std::string m_companyAddr;
	int m_employees = 0;
};
}  // namespace DataGraph::Forms
#endif
