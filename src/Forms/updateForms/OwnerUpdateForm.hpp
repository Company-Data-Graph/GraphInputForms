#ifndef DATAGRAPHTOOL_FORMS_UPDATEFORMS_OWNERUPDATEFORM_H__
#define DATAGRAPHTOOL_FORMS_UPDATEFORMS_OWNERUPDATEFORM_H__

#include <Forms/Form.hpp>
#include <string_view>

namespace DataGraph::Forms
{
class OwnerUpdate: public Form
{
public:
	int init() final;
	int draw() final;
	const char* name() const final;
	void reset() final;

private:
	std::string_view m_errorMessage;
};
}  // namespace DataGraph::Forms
#endif
