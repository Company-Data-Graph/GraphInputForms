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

private:
	uint32_t m_errorCode;
	std::string_view m_errorMessage;

	std::string m_departmentName;
};
}  // namespace DataGraph::Forms
#endif
