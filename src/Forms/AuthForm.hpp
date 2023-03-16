#ifndef DATAGRAPHTOOL_FORMS_AUTHFORM_H__
#define DATAGRAPHTOOL_FORMS_AUTHFORM_H__

#include <pgfe/pgfe.hpp>
#include <string_view>
#include <memory>

#include <core/Networking.hpp>

namespace DataGraph::Forms
{
class AuthForm
{
public:
	int init();
	int draw();

private:
	Networking::ConnectionData m_database;
	Networking::ConnectionData m_mediaServer;

	int m_errorCode;
	std::string m_errorMessage;
};
}  // namespace DataGraph::Forms

#endif
