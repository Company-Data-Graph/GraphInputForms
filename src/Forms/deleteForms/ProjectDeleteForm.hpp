#ifndef DATAGRAPHTOOL_FORMS_DELETEFORMS_PROJECTDELETEFORM_H__
#define DATAGRAPHTOOL_FORMS_DELETEFORMS_PROJECTDELETEFORM_H__

#include <string_view>
#include <Forms/Form.hpp>

namespace DataGraph::Forms
{
class ProjectDelete: public Form
{
public:
	int init() override final;
	int draw() override final;
	const char* name() const override final;
	void reset() final;

private:
	uint32_t m_errorCode;
	std::string_view m_errorMessage;

	uint32_t m_projectId = -1;
	std::string m_projectName;
	std::string m_projectNameSearch;

	uint32_t m_nodeId = -1;
	std::string m_projectVersion;
	std::string m_projectVersionSearch;
};
}  // namespace DataGraph::Forms
#endif
