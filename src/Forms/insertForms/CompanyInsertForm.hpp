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
	std::string_view m_errorMessage;
};
}  // namespace DataGraph::Forms
#endif
