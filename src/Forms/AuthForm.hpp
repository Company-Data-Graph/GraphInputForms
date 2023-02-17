#ifndef DATAGRAPHTOOL_FORMS_AUTHFORM_H__
#define DATAGRAPHTOOL_FORMS_AUTHFORM_H__

#include <pgfe/pgfe.hpp>
#include <string_view>
#include <memory>
struct ConnectionData
{
	std::string ip;
	std::string port;
	std::string login;
	std::string password;
};

namespace DataGraph::Forms
{
class AuthForm
{
public:
	int init();
	int draw();

private:
	ConnectionData m_database;
	ConnectionData m_mediaServer;

	int m_errorCode;
	std::string m_errorMessage;
};
}  // namespace DataGraph::Forms

#endif
