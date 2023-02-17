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

	// Split the header string into lines
	while (!headerView.empty())
	{
		// Find the first '\r\n' sequence in the string
		std::size_t separatorPos = headerView.find("\r\n");
		if (separatorPos == std::string::npos)
		{
			return false;
		}

		// Extract the line from the string
		std::string_view line = headerView.substr(0, separatorPos);
		headerView.remove_prefix(separatorPos + 2);

		// Find the first ':' character in the line
		separatorPos = line.find(':');
		if (separatorPos == std::string::npos)
		{
			FormHandler::logs()->error("Networking", "Error: Invalid header line format: {}", line);
			return false;
		}

		// Extract the key and value from the line
		std::string_view key = line.substr(0, separatorPos);
		std::string_view value = line.substr(separatorPos + 1);

		// Store the key-value pair in the header JSON object
		header.emplace(key, value);
	}

	return header;
}

bool receiveResponse(SOCKET socket, Response& response)
{
	constexpr const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];
	int result;

	// Receive the response
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
		}
		else
		{
			FormHandler::logs()->error("Networking", "Error receiving response: {}", WSAGetLastError());
			return false;
		}
	} while (result > 0 && !(result < BUFFER_SIZE));

	// Extract the return code and header
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

	std::string_view headerView(&responseView[headerPos + 2], bodyPos - headerPos - 2);
	// Parse the header and body into JSON objects
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
	// Parse the domain name and endpoint from the URL
	size_t domainStart = url.find("//") + 2;
	size_t domainEnd = url.find('/', domainStart);
	size_t domainNameLength = domainEnd - domainStart;
	std::string_view domainName(&url[domainStart], domainNameLength);

	size_t endpointStart = domainEnd;
	std::string_view endpoint(&url[endpointStart], url.length() - endpointStart);

	// Construct the HTTP request
	static const std::string_view requestTemplate = R"({} {} HTTP/1.1
Host: {}
{}Content-Type: text/plain
Content-Length: {}

{})";
	const std::string_view requestType = isPost ? "POST" : "GET";
	const std::string requestString
		 = fmt::format(requestTemplate, requestType, endpoint, domainName, createHeadersString(headers), body.dump().length(), body.dump());

	// Send the request
	int rresult = sendAll(socket, requestString.data(), requestString.size(), 0);
	if (rresult == SOCKET_ERROR)
	{
		FormHandler::logs()->error("Networking", "Error sending request: {}", WSAGetLastError());
		return false;
	}

	// Receive the response
	if (!receiveResponse(socket, response))
	{
		return false;
	}

	return true;
}

SOCKET createSocket(const std::string_view& address, const std::string_view& port)
{
	// Resolve the server address and port
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

	// Create a socket
	SOCKET sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sock == INVALID_SOCKET)
	{
		FormHandler::logs()->error("Networking", "Error creating socket: {}", WSAGetLastError());
		freeaddrinfo(result);
		return INVALID_SOCKET;
	}

	return sock;
}

bool connectSocket(SOCKET socket, const std::string_view& address, const std::string_view& port)
{
	// Connect to the server
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
		FormHandler::logs()->error("Networking", "Error connecting to server: {}", WSAGetLastError());
		freeaddrinfo(result);
		return false;
	}

	freeaddrinfo(result);
	return true;
}

bool connectToMediaServer(ConnectionData& mediaServer, Response& response)
{
	// Create a socket
	SOCKET socket = createSocket(mediaServer.ip, mediaServer.port);
	if (socket == INVALID_SOCKET)
	{
		FormHandler::logs()->error("Networking", "MediaServer socket creation has failed.");
		return false;
	}

	// Connect to the server
	if (!connectSocket(socket, mediaServer.ip, mediaServer.port))
	{
		closesocket(socket);
		FormHandler::logs()->error("Networking", "MediaServer socket connection has failed.");
		return false;
	}

	std::string mediaServerURL = fmt::format("{}:{}", mediaServer.ip, mediaServer.port);
	// Send a request to the server
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

	// Create a socket
	SOCKET psocket = createSocket(proxyServerIp, proxyServerPort);
	if (psocket == INVALID_SOCKET)
	{
		FormHandler::logs()->error("Networking", "ProxyServer socket creation has failed.");
		return 2;
	}

	// Connect to the server
	if (!connectSocket(psocket, proxyServerIp, proxyServerPort))
	{
		FormHandler::logs()->error("Networking", "ProxyServer socket connection has failed.");

		closesocket(psocket);
		return 3;
	}

	// Send a request to the server
	// Response response;
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

	// Clean up
	closesocket(psocket);

	return 0;
}
}  // namespace DataGraph::Networking
