#ifndef DATAGRAPHTOOL_FORMS_DELETEFORMS_H__
#define DATAGRAPHTOOL_FORMS_DELETEFORMS_H__

#include "Form.hpp"

#include <vector>
#include <string_view>

namespace DataGraph::Forms
{
class DeleteForm: public Form
{
public:
	int init() override final;
	int draw() override final;
	const char* name() const override final;
	void reset() override final;

private:
	std::vector<Form*> subForms;
};
}  // namespace DataGraph::Forms
#endif
