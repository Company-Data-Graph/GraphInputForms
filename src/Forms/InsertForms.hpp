#ifndef DATAGRAPHTOOL_FORMS_PROJECTFORM_H__
#define DATAGRAPHTOOL_FORMS_PROJECTFORM_H__

#include "Form.hpp"

#include <list>
#include <string_view>

namespace DataGraph::Forms
{
class InsertForm: public Form
{
public:
	int init() override final;
	int draw() override final;
	const char* name() const override final;
	void reset() override final;

private:
	std::list<Form*> subForms;
};
}  // namespace DataGraph::Forms
#endif
