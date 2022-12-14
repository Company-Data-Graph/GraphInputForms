#ifndef DATAGRAPHTOOL_FORMS_DELETEFORMS_DepartmentSDELETEFORM_H__
#define DATAGRAPHTOOL_FORMS_DELETEFORMS_DepartmentSDELETEFORM_H__

#include <string_view>
#include <Forms/Form.hpp>
namespace DataGraph::Forms
{
class DepartmentDelete: public Form
{
public:
	int init() final;
	int draw() final;
	const char* name() const final;
	void reset() override final;

private:
	int m_errorCode;
	std::string_view m_errorMessage;
};
}  // namespace DataGraph::Forms
#endif
