#ifndef DATAGRAPHTOOL_CORE_NETWORKING_H__
#define DATAGRAPHTOOL_CORE_NETWORKING_H__

#include <string_view>
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

bool connectToMediaServer(ConnectionData& mediaServer, Response& response);

int loadFileToHost(std::string_view filePath,
	 ConnectionData& mediaServer,
	 Response& response);
}  // namespace DataGraph::Networking
#endif
