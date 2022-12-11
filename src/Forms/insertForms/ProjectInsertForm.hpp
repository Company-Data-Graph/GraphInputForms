#ifndef DATAGRAPHTOOL_FORMS_INSERTFORMS_PROJECTINSERTFORM_H__
#define DATAGRAPHTOOL_FORMS_INSERTFORMS_PROJECTINSERTFORM_H__

#include <string_view>
#include <Forms/Form.hpp>

namespace DataGraph::Forms
{
class ProjectInsert: public Form
{
public:
	int init() override final;
	int draw() override final;
	const char* name() const override final;
	void reset() final;

private:
	std::string_view m_errorMessage;
};
}  // namespace DataGraph::Forms
#endif
