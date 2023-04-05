#ifndef DATAGRAPHTOOL_FORMS_AUTHFORM_H__
#define DATAGRAPHTOOL_FORMS_AUTHFORM_H__

#include <pgfe/pgfe.hpp>
#include <string_view>
#include <memory>

#include <Networking/ConnectionData.hpp>

namespace DataGraph::Forms
{
class AuthForm
{
public:
	int init();
	int draw();

	std::string_view getStatusMessage() const;
	int getStatusCode() const;

private:
	Networking::ConnectionData m_database;
	Networking::ConnectionData m_mediaServer;

	int m_errorCode = -1;
	std::string m_errorMessage;
};
}  // namespace DataGraph::Forms

#endif
