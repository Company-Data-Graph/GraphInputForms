#ifndef DATAGRAPHTOOL_CORE_NETWORKING_DATA_H__
#define DATAGRAPHTOOL_CORE_NETWORKING_DATA_H__

#include <string>
#include <json/json.hpp>

namespace DataGraph::Networking
{
struct ConnectionData
{
	std::string ip;
	std::string port;
	std::string login;
	std::string password;
};

struct Response
{
	int returnCode = 0;
	nlohmann::json header;
	nlohmann::json body;
	std::string raw;
};
}  // namespace DataGraph::Networking
#endif
