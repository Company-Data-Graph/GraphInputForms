#ifndef DATAGRAPHTOOL_FORMS_INSERTFORMS_DepartmentSINSERTFORM_H__
#define DATAGRAPHTOOL_FORMS_INSERTFORMS_DepartmentSINSERTFORM_H__

#include <string_view>
#include <Forms/Form.hpp>
namespace DataGraph::Forms
{
class DepartmentInsert: public Form
{
public:
	int init() final;
	int draw() final;
	const char* name() const final;
	void reset() final;

	std::string_view getStatusMessage() const override final;
	int getStatusCode() const override final;

private:
	uint32_t m_errorCode = -1;
	std::string_view m_errorMessage;

	std::string m_departmentName;
};
}  // namespace DataGraph::Forms
#endif
