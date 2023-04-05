#ifndef DATAGRAPHTOOL_FORMS_UPDATEFORMS_H__
#define DATAGRAPHTOOL_FORMS_UPDATEFORMS_H__

#include "Form.hpp"

#include <list>
#include <string_view>

namespace DataGraph::Forms
{
class UpdateForm: public Form
{
public:
	int init() override final;
	int draw() override final;
	const char* name() const override final;
	void reset() override final;

	std::string_view getStatusMessage() const override final;
	int getStatusCode() const override final;

private:
	std::list<Form*> subForms;
	std::string_view m_activeForm;
};
}  // namespace DataGraph::Forms
#endif
