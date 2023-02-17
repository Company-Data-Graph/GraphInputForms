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
	uint32_t m_errorCode;
	std::string_view m_errorMessage;

	uint32_t m_ownerId = -1;
	std::string m_ownerNamePreview;
	std::string m_ownerSearch;

	std::string m_ownerNewName;
};
}  // namespace DataGraph::Forms
#endif
