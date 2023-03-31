#ifndef DATAGRAPHTOOL_NETWORKING_BACKEND_CONNECTION_H__
#define DATAGRAPHTOOL_NETWORKING_BACKEND_CONNECTION_H__

#include <string>
#include <string_view>
#include <unordered_map>

#include <json/json.hpp>

#include <Networking/ConnectionData.hpp>

#include "SocketWrapper.hpp"

namespace DataGraph::Networking::backend
{
using json = nlohmann::json;
using ErrorCodes = uint16_t;
namespace ErrorCode
{
enum : uint16_t
{
	OK = 0,
	FILE_NOT_FOUND,
	INVALID_FILE_SIZES,
	FILE_READ_ERROR,
	INVALID_PARAMS,
	SEND_FAILED,
	NO_LINE_SEPARATOR_FOUND,
	NO_VALUE_SEPARATOR_FOUND,
	CONNECTION_CLOSED,
	RECEIVE_ERROR,
	NO_RETURN_CODE_FOUND,
	NO_RESPONSE_BODY_FOUND,
	BODY_PARSE_FAILED,
	CONNECTION_TIMEOUT,
	WAITING_FOR_CONNECTION_ERROR,
	CONNECTION_FAILED,
	SOCKET_OPTIONS_RECEIVE_FAILED,
	SOCKET_CREATION_FAILED
};
}
bool isOK(ErrorCodes errorCodes);

std::string createHeadersString(const std::unordered_map<std::string, std::string>& headers);
[[nodiscard]] ErrorCodes readFile(const std::string_view& filePath, std::vector<char>& fileContents);
[[nodiscard]] ErrorCodes parseHeader(std::string_view headerView, json& header);
[[nodiscard]] ErrorCodes receiveResponse(SocketWrapper& socket, Response& response);
[[nodiscard]] ErrorCodes sendAll(SocketWrapper& socket, const char* buffer, int length, int flags, int* sentBytes = nullptr);
[[nodiscard]] ErrorCodes sendRequest(SocketWrapper& socket,
	 const std::string& url,
	 const std::unordered_map<std::string, std::string>& headers,
	 const json& body,
	 Response& response,
	 bool isPost = true);
[[nodiscard]] SocketWrapper createSocket(const std::string_view& address, const std::string_view& port, ErrorCodes* status = nullptr);
[[nodiscard]] ErrorCodes connectSocket(SocketWrapper& socket, const std::string_view& address, const std::string_view& port, int timeout);
[[nodiscard]] ErrorCodes connectToMediaServer(ConnectionData& mediaServer, Response& response);
std::string_view getContentType(std::string_view filename);
[[nodiscard]] ErrorCodes loadFileToHost(std::string_view filePath, ConnectionData& mediaServer, Response& response);

};	// namespace DataGraph::Networking::backend
#endif
