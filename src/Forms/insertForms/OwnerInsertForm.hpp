#ifndef DATAGRAPHTOOL_FORMS_INSERTFORMS_OWNERINSERTFORM_H__
#define DATAGRAPHTOOL_FORMS_INSERTFORMS_OWNERINSERTFORM_H__

#include <Forms/Form.hpp>
#include <string>
#include <string_view>

namespace DataGraph::Forms
{
class OwnerInsert: public Form
{
public:
	int init() final;
	int draw() final;
	const char* name() const final;
	void reset() final;

	std::string_view getStatusMessage() const override final;
	int getStatusCode() const override final;

private:
	std::string_view m_errorMessage;

	int m_errorCode = 1;
	std::string m_ownerName;
};
}  // namespace DataGraph::Forms
#endif
