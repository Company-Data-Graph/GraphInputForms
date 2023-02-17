#ifndef DATAGRAPHTOOL_FORMS_UPDATEFORMS_DepartmentSUPDATEFORM_H__
#define DATAGRAPHTOOL_FORMS_UPDATEFORMS_DepartmentSUPDATEFORM_H__

#include <string_view>
#include <Forms/Form.hpp>
namespace DataGraph::Forms
{
class DepartmentUpdate: public Form
{
public:
	int init() final;
	int draw() final;
	const char* name() const final;
	void reset() final;

private:
	uint32_t m_errorCode;
	std::string_view m_errorMessage;

	uint32_t m_departmentId = -1;
	std::string m_newDepartmentName;
};
}  // namespace DataGraph::Forms
#endif
