#include "Networking.hpp"

#include "backend/Connection.hpp"

namespace DataGraph::Networking
{
bool connectToMediaServer(ConnectionData& mediaServer, Response& response)
{
	auto result = backend::connectToMediaServer(mediaServer, response);
	return backend::isOK(result);
}
bool loadFileToHost(std::string_view filePath, ConnectionData& mediaServer, Response& response)
{
	auto result = backend::loadFileToHost(filePath, mediaServer, response);
	return backend::isOK(result);
}
}  // namespace DataGraph::Networking
