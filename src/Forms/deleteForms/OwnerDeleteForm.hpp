#ifndef DATAGRAPHTOOL_FORMS_DELETEFORMS_OWNERDELETEFORM_H__
#define DATAGRAPHTOOL_FORMS_DELETEFORMS_OWNERDELETEFORM_H__

#include <Forms/Form.hpp>
#include <string_view>

namespace DataGraph::Forms
{
class OwnerDelete: public Form
{
public:
	int init() final;
	int draw() final;
	const char* name() const final;
	void reset() final;

	std::string_view getStatusMessage() const override final;
	int getStatusCode() const override final;

private:
	std::string m_companyErrorName;
	std::string_view m_errorMessage;

	int m_errorCode = -1;

	uint32_t m_ownerId = -1;
	std::string m_ownerNamePreview;
	std::string m_ownerSearch;
};
}  // namespace DataGraph::Forms
#endif
