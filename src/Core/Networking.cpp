#include "Networking.hpp"

#include <unordered_map>
#include <string>
#include <string_view>
#include <ws2tcpip.h>
#include <Windows.h>
#include <json/json.hpp>

#include <Core/FormHandler.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/color.h>

namespace DataGraph::Networking
{
using json = nlohmann::json;

std::string createHeadersString(const std::unordered_map<std::string, std::string>& headers)
{
	std::string headersString;
	for (auto& [name, value] : headers)
	{
		headersString += fmt::format("{}: {}\r\n", name, value);
	}

	return headersString;
}

enum class ErrorCodes
{
	SUCCESS = 0,
	INVALID_EXTENSION,
	FILE_NOT_FOUND,
	INVALID_FILE,
	MAPPING_ERROR,
	IO_ERROR,
	INVALID_FILE_SIZES,
	FILE_READ_ERROR
};

ErrorCodes readFile(const std::string_view& filePath, std::vector<char>& fileContents)
{
	std::ifstream file(filePath.data(), std::ios::binary);
	if (!file)
	{
		return ErrorCodes::FILE_NOT_FOUND;
	}

	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	if (size < 0)
	{
		return ErrorCodes::INVALID_FILE_SIZES;
	}

	file.seekg(0, std::ios::beg);
	fileContents.resize(size);
	if (!file.read(fileContents.data(), size))
	{
		return ErrorCodes::FILE_READ_ERROR;
	}

	return ErrorCodes::SUCCESS;
}

json parseHeader(std::string_view headerView)
{
	json header;

	while (!headerView.empty())
	{
		std::size_t separatorPos = headerView.find("\r\n");
		if (separatorPos == std::string::npos)
		{
			return {};
		}

		std::string_view line = headerView.substr(0, separatorPos);
		headerView.remove_prefix(separatorPos + 2);

		separatorPos = line.find(':');
		if (separatorPos == std::string::npos)
		{
			FormHandler::logs()->error("Networking", "Error: Invalid header line format: {}", line);
			return {};
		}

		std::string_view key = line.substr(0, separatorPos);
		std::string_view value = line.substr(separatorPos + 2);

		header.emplace(key, value);
	}

	return header;
}

bool receiveResponse(SOCKET socket, Response& response)
{
	constexpr const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];
	int result;

	response.header.clear();
	response.body.clear();
	response.returnCode = 0;

	std::string responseStr;
	do
	{
		result = recv(socket, buffer, BUFFER_SIZE, 0);
		if (result > 0)
		{
			responseStr.append(buffer, result);
		}
		else if (result == 0)
		{
			FormHandler::logs()->error("Networking", "Connection closed");
			return false;
		}
		else
		{
			FormHandler::logs()->error("Networking", "Error receiving response: {}", WSAGetLastError());
			return false;
		}
	} while (result > 0 && !(result < BUFFER_SIZE));

	std::string_view responseView(responseStr);
	std::size_t headerPos = responseView.find("\r\n");

	uint32_t numPos = responseView.find(' ');
	response.returnCode = atoi(&responseView[numPos]);

	if (headerPos == std::string::npos)
	{
		FormHandler::logs()->error("Networking", "Error: Failed to locate begin of header");
		return false;
	}

	std::size_t bodyPos = responseView.find("\r\n\r\n", headerPos + 2);
	if (bodyPos == std::string::npos)
	{
		FormHandler::logs()->error("Networking", "Error: Failed to locate end of header");
		return false;
	}

	std::string_view headerView(&responseView[headerPos + 2], bodyPos - headerPos);
	try
	{
		response.header = parseHeader(headerView);
		if (bodyPos + 4 < responseView.size())
		{
			std::string_view bodyView(&responseView[bodyPos + 4], responseView.size() - (bodyPos + 4));
			response.body = nlohmann::json::parse(bodyView.data());
		}
		else
		{
			response.body.clear();
		}
	}
	catch (const std::exception& e)
	{
		FormHandler::logs()->error("Networking", "Error parsing header or body: {}", e.what());
		return false;
	}

	return true;
}

int sendAll(SOCKET socket, const char* buffer, int length, int flags)
{
	if (length <= 0 || socket == INVALID_SOCKET || buffer == nullptr)
	{
		return SOCKET_ERROR;
	}

	int totalSent = 0;
	while (totalSent < length)
	{
		int sent = send(socket, buffer + totalSent, length - totalSent, flags);
		if (sent == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		totalSent += sent;
	}
	return totalSent;
}

bool send_request(SOCKET socket,
	 const std::string& url,
	 const std::unordered_map<std::string, std::string>& headers,
	 const json& body,
	 Response& response,
	 bool isPost = true)
{
	size_t domainStart = url.find("//") + 2;
	size_t domainEnd = url.find('/', domainStart);
	if (domainEnd == -1)
	{
		domainEnd = url.size();
	}

	size_t domainNameLength = domainEnd - domainStart;
	std::string_view domainName(&url[domainStart], domainNameLength);

	size_t endpointStart = domainEnd;
	std::string_view endpoint(&url[endpointStart], url.length() - endpointStart);

	static const std::string_view requestTemplate = R"({} {} HTTP/1.1
Host: {}
{}Content-Type: text/plain
Content-Length: {}

{})";
	const std::string_view requestType = isPost ? "POST" : "GET";
	const std::string requestString
		 = fmt::format(requestTemplate, requestType, endpoint, domainName, createHeadersString(headers), body.dump().length(), body.dump());

	int result = sendAll(socket, requestString.data(), requestString.size(), 0);
	if (result == SOCKET_ERROR)
	{
		FormHandler::logs()->error("Networking", "Error sending request: {}", WSAGetLastError());
		return false;
	}

	if (!receiveResponse(socket, response))
	{
		return false;
	}

	return true;
}

SOCKET createSocket(const std::string_view& address, const std::string_view& port)
{
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	addrinfo* result = nullptr;
	int res = getaddrinfo(address.data(), port.data(), &hints, &result);
	if (res != 0)
	{
		FormHandler::logs()->error("Networking", "Error resolving address: 0x%08X", (int)gai_strerror(res));
		return INVALID_SOCKET;
	}

	SOCKET sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sock == INVALID_SOCKET)
	{
		FormHandler::logs()->error("Networking", "Error creating socket: {}", WSAGetLastError());
		freeaddrinfo(result);
		return INVALID_SOCKET;
	}

	return sock;
}

bool connectSocket(SOCKET socket, const std::string_view& address, const std::string_view& port, int timeout)
{
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo* result = nullptr;
	int res = getaddrinfo(address.data(), port.data(), &hints, &result);
	if (res != 0)
	{
		FormHandler::logs()->error("Networking", "Error resolving address : 0x%08X", (int)gai_strerror(res));
		return false;
	}

	res = connect(socket, result->ai_addr, static_cast<int>(result->ai_addrlen));
	if (res == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS)
		{
			FormHandler::logs()->error("Networking", "Error connecting to server: {}", err);
			freeaddrinfo(result);
			return false;
		}

		fd_set writeSet;
		FD_ZERO(&writeSet);
		FD_SET(socket, &writeSet);

		timeval timeoutVal;
		timeoutVal.tv_sec = timeout;
		timeoutVal.tv_usec = 0;

		res = select(0, nullptr, &writeSet, nullptr, &timeoutVal);
		if (res == SOCKET_ERROR)
		{
			FormHandler::logs()->error("Networking", "Error waiting for connection: {}", WSAGetLastError());
			freeaddrinfo(result);
			return false;
		}
		else if (res == 0)
		{
			FormHandler::logs()->error("Networking", "Connection timeout");
			freeaddrinfo(result);
			return false;
		}

		int optval;
		int optlen = sizeof(optval);
		res = getsockopt(socket, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen);
		if (res == SOCKET_ERROR)
		{
			FormHandler::logs()->error("Networking", "Error getting socket options: {}", WSAGetLastError());
			freeaddrinfo(result);
			return false;
		}
		else if (optval != 0)
		{
			FormHandler::logs()->error("Networking", "Error connecting to server: {}", optval);
			freeaddrinfo(result);
			return false;
		}
	}

	freeaddrinfo(result);
	return true;
}

bool connectToMediaServer(ConnectionData& mediaServer, Response& response)
{
	SOCKET socket = createSocket(mediaServer.ip, mediaServer.port);
	if (socket == INVALID_SOCKET)
	{
		FormHandler::logs()->error("Networking", "MediaServer socket creation has failed.");
		return false;
	}

	if (!connectSocket(socket, mediaServer.ip, mediaServer.port, 5))
	{
		closesocket(socket);
		FormHandler::logs()->error("Networking", "MediaServer socket connection has failed.");
		return false;
	}

	std::string mediaServerURL = fmt::format("{}:{}", mediaServer.ip, mediaServer.port);
	bool success = send_request(socket, fmt::format("http://{}/media-server/signin", mediaServerURL), {},
		 json({{"username", mediaServer.login}, {"password", mediaServer.password}}), response);

	closesocket(socket);

	return success;
}

int loadFileToHost(std::string_view filePath,
	 ConnectionData& mediaServer,
	 std::string_view proxyServerIp,
	 std::string_view proxyServerPort,
	 Response& response)
{
	bool success = connectToMediaServer(mediaServer, response);
	if (!success || response.returnCode != 200)
	{
		return 1;
	}

	SOCKET psocket = createSocket(proxyServerIp, proxyServerPort);
	if (psocket == INVALID_SOCKET)
	{
		FormHandler::logs()->error("Networking", "ProxyServer socket creation has failed.");
		return 2;
	}

	if (!connectSocket(psocket, proxyServerIp, proxyServerPort, 5))
	{
		FormHandler::logs()->error("Networking", "ProxyServer socket connection has failed.");

		closesocket(psocket);
		return 3;
	}

	success = send_request(psocket, fmt::format("http://{}:{}/loading", proxyServerIp, proxyServerPort), {},
		 json({{"mediaServerDest", fmt::format("http://{}/media-server/upload", fmt::format("{}:{}", mediaServer.ip, mediaServer.port))},
			  {"token", response.body["token"].get<std::string>()}, {"filePath", filePath}}),
		 response);
	if (!success)
	{
		FormHandler::logs()->error("Networking", "File loading has failed.");

		closesocket(psocket);
		return 4;
	}

	closesocket(psocket);

	return 0;
}
}  // namespace DataGraph::Networking
