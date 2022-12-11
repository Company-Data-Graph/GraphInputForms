#ifndef DATAGRAPHTOOL_FORMS_AUTHFORM_H__
#define DATAGRAPHTOOL_FORMS_AUTHFORM_H__

#include <pgfe/pgfe.hpp>
#include <string_view>
#include <memory>
namespace dmitigr::pgfe
{
class Connection;
}

namespace DataGraph::Forms
{
class AuthForm
{
public:
	int init();
	int draw();

private:
	std::unique_ptr<dmitigr::pgfe::Connection> m_conn;

	std::string m_databaseIP;
	std::string m_databasePort;
	std::string m_databaseLogin;
	std::string m_databasePassword;

	std::string m_mediaServerIP;
	std::string m_mediaServerPort;
	std::string m_mediaServerLogin;
	std::string m_mediaServerPassword;

	int m_errorCode;
	std::string m_errorMessage;
};
}  // namespace DataGraph::Forms

#endif
