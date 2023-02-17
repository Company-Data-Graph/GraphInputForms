#ifndef DATAGRAPHTOOL_CORE_NETWORKING_H__
#define DATAGRAPHTOOL_CORE_NETWORKING_H__

#include <string_view>
#include <json/json.hpp>

#include <Forms/AuthForm.hpp>

namespace DataGraph::Networking
{
struct Response
{
	int returnCode = 0;
	nlohmann::json header;
	nlohmann::json body;
};

bool connectToMediaServer(ConnectionData& mediaServer, Response& response);

int loadFileToHost(std::string_view filePath,
	 ConnectionData& mediaServer,
	 std::string_view proxyServerIp,
	 std::string_view proxyServerPort,
	 Response& response);
}  // namespace DataGraph::Networking
#endif
