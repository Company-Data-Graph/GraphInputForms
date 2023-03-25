#ifndef DATAGRAPHTOOL_CORE_NETWORKING_H__
#define DATAGRAPHTOOL_CORE_NETWORKING_H__

#include <string_view>
#include "ConnectionData.hpp"

namespace DataGraph::Networking
{
bool connectToMediaServer(ConnectionData& mediaServer, Response& response);

bool loadFileToHost(std::string_view filePath, ConnectionData& mediaServer, Response& response);
}  // namespace DataGraph::Networking
#endif
