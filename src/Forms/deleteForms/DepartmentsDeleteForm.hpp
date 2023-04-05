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

	std::string_view getStatusMessage() const override final;
	int getStatusCode() const override final;

private:
	int m_errorCode = -1;
	std::string_view m_errorMessage;

	uint32_t m_departmentId = -1;
	std::string m_departmentPreview;
};
}  // namespace DataGraph::Forms
#endif
