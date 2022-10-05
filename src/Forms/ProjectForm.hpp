#ifndef DATAGRAPHTOOL_FORMS_PROJECTFORM_H__
#define DATAGRAPHTOOL_FORMS_PROJECTFORM_H__

#include "Form.hpp"

namespace DataGraph::Forms
{
class ProjectForm: public Form
{
	int init() override final;
	int draw() override final;
};
}  // namespace DataGraph::Forms
#endif
