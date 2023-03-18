#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include <json/json.hpp>
#include <ws2tcpip.h>
#include <Windows.h>
#include <fmt/format.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include <atomic>
#include <future>
#include <Core/Networking.hpp>

#include <stb_image.h>
#include <utils/Encrypt.hpp>

#include <pgfe/pgfe.hpp>

namespace DataGraph::Networking
{
std::string createHeadersString(const std::unordered_map<std::string, std::string>& headers);
nlohmann::json parseHeader(std::string_view headerView);
SOCKET createSocket(const std::string_view& address, const std::string_view& port);
bool connectSocket(SOCKET socket, const std::string_view& address, const std::string_view& port, int timeout);
int sendAll(SOCKET socket, const char* buffer, int length, int flags, int* sentBytes = nullptr);
bool receiveResponse(SOCKET socket, Response& response);
bool send_request(SOCKET socket,
	 const std::string& url,
	 const std::unordered_map<std::string, std::string>& headers,
	 const nlohmann::json& body,
	 Response& response,
	 bool isPost);

using ErrorCodes = uint16_t;
namespace ErrorCode
{
enum : uint16_t
{
	SUCCESS = 0,
	FILE_NOT_FOUND,
	INVALID_FILE_SIZES,
	FILE_READ_ERROR
};
}
ErrorCodes readFile(const std::string_view& filePath, std::vector<char>& fileContents);
}  // namespace DataGraph::Networking

using namespace DataGraph;
using namespace nlohmann;
TEST(CreateHeadersStringTest, EmptyHeaders)
{
	std::unordered_map<std::string, std::string> headers;
	std::string result = Networking::createHeadersString(headers);
	EXPECT_EQ(result, "");
}

TEST(CreateHeadersStringTest, SingleHeader)
{
	std::unordered_map<std::string, std::string> headers = {{"Content-Type", "text/plain"}};
	std::string result = Networking::createHeadersString(headers);
	EXPECT_EQ(result, "Content-Type: text/plain\r\n");
}

TEST(CreateHeadersStringTest, MultipleHeaders)
{
	std::unordered_map<std::string, std::string> headers
		 = {{"Content-Type", "text/html"}, {"Accept-Language", "en-US"}, {"Authorization", "Bearer abcdef1234567890"}};
	std::string result = Networking::createHeadersString(headers);
	EXPECT_EQ(result, "Content-Type: text/html\r\nAuthorization: Bearer abcdef1234567890\r\nAccept-Language: en-US\r\n");
}

TEST(ParseHeader, ValidHeader)
{
	std::string contentType = "application/json";
	std::string authorization = "Bearer abc123";
	std::string header = fmt::format("Content-Type: {}\r\nAuthorization: {}\r\n", contentType, authorization);
	json result = Networking::parseHeader(header);
	ASSERT_TRUE(result.contains("Content-Type"));
	ASSERT_TRUE(result.contains("Authorization"));
	EXPECT_EQ(result["Content-Type"], contentType);
	EXPECT_EQ(result["Authorization"], authorization);
}

TEST(ParseHeader, MalformedHeaderMissingColon)
{
	std::string_view header = "Content-Type: application/json\r\nAuthorization Bearer abc123\r\n";
	EXPECT_TRUE(Networking::parseHeader(header).empty());
}

TEST(ParseHeader, MalformedHeaderMissingSeparator)
{
	std::string_view header = "Content-Type: application/jsonAuthorization: Bearer abc123\r\n";
	EXPECT_FALSE(Networking::parseHeader(header).contains("Authorization"));
}

TEST(ParseHeader, EmptyHeader)
{
	std::string_view header = "";
	json result = Networking::parseHeader(header);
	EXPECT_TRUE(result.empty());
}

TEST(CreateSocketTest, ValidAddressAndPort)
{
	const std::string_view address = "localhost";
	const std::string_view port = "80";

	SOCKET sock = Networking::createSocket(address, port);
	ASSERT_NE(sock, INVALID_SOCKET);

	closesocket(sock);
}

TEST(NetworkingTest, CreateSocketInvalidAddressReturnsInvalidSocket)
{
	std::string_view invalidAddress = "invalidAddress";
	std::string_view validPort = "80";

	SOCKET socket = Networking::createSocket(invalidAddress, validPort);

	ASSERT_EQ(socket, INVALID_SOCKET);
}

class ReadFileTest: public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		m_testDir = "tmp-test";
		std::filesystem::create_directory(m_testDir);
	}

	virtual void TearDown() override { std::filesystem::remove_all(m_testDir); }

public:
	static std::string getPath(const std::string& path) { return m_testDir.string() + '\\' + path; }
	static std::filesystem::path m_testDir;
};

std::filesystem::path ReadFileTest::m_testDir;

TEST(ReadFileTest, NonexistentFile)
{
	std::vector<char> fileContents;
	EXPECT_EQ(Networking::ErrorCode::FILE_NOT_FOUND, Networking::readFile(ReadFileTest::getPath("nonexistent_file.txt"), fileContents));
}

TEST(ReadFileTest, EmptyFile)
{
	auto filePath = ReadFileTest::getPath("EmptyFile.txt");
	std::ofstream file(filePath);
	file.close();

	std::vector<char> fileContents;
	EXPECT_EQ(Networking::ErrorCode::SUCCESS, Networking::readFile(filePath, fileContents));
	EXPECT_EQ(0, fileContents.size());
}

TEST(ReadFileTest, SmallFile)
{
	auto filePath = ReadFileTest::getPath("SmallFile.txt");
	std::ofstream file(filePath);

	std::string fileData = "THIS IS TEST MESSAGE!";
	file << fileData;
	file.close();

	std::vector<char> fileContents;
	EXPECT_EQ(Networking::ErrorCode::SUCCESS, Networking::readFile(filePath, fileContents));
	EXPECT_EQ(fileData.size(), fileContents.size());
}

TEST(ReadFileTest, LargeFile)
{
	auto filePath = ReadFileTest::getPath("LargeFile.txt");
	std::ofstream file(filePath);
	std::string fileData = "This is a line of text.\n";
	uint32_t repeateCounter = 1000000;
	for (uint32_t i = 0; i < repeateCounter; ++i)
	{
		file << fileData;
	}
	int size = file.tellp();
	file.close();

	std::vector<char> fileContents;
	EXPECT_EQ(Networking::ErrorCode::SUCCESS, Networking::readFile(filePath, fileContents));
	EXPECT_EQ(size, fileContents.size());
}


TEST(ConnectSocketTest, connectionSuccessfully)
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	std::string serverIp = "127.0.0.1";
	std::string serverPort = "12345";
	SOCKET serverSocket = Networking::createSocket(serverIp, serverPort);
	if (serverSocket == INVALID_SOCKET)
	{
		WSACleanup();

		FAIL() << "Failed to create server socket";
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(stoi(serverPort));
	inet_pton(AF_INET, serverIp.c_str(), &(serverAddress.sin_addr));
	int bindResult = bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
	if (bindResult == SOCKET_ERROR)
	{
		closesocket(serverSocket);
		WSACleanup();

		FAIL() << "Failed to bind server socket";
	}

	listen(serverSocket, SOMAXCONN);

	SOCKET clientSocket = Networking::createSocket(serverIp, serverPort);
	ASSERT_NE(clientSocket, INVALID_SOCKET) << "Failed to create client socket";

	bool connected = Networking::connectSocket(clientSocket, serverIp, serverPort, 5);
	EXPECT_TRUE(connected);

	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();
}

TEST(ConnectSocketTest, FailsToConnect)
{
	const std::string_view address = "invalidAddress";
	const std::string_view port = "12345";

	SOCKET testSocket = Networking::createSocket(address, port);

	bool result = Networking::connectSocket(testSocket, address, port, 5);

	EXPECT_FALSE(result);

	closesocket(testSocket);
}

TEST(ConnectSocketTest, FailsToConnectSocket)
{
	const std::string_view address = "invalidAddress";
	const std::string_view port = "12345";

	SOCKET testSocket = INVALID_SOCKET;

	bool result = Networking::connectSocket(testSocket, address, port, 5);
	std::unique_ptr<SOCKET, int (*)(SOCKET*)> lSocket(&testSocket, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	EXPECT_FALSE(result);
}

TEST(ConnectSocketTest, ConnectionTimeout)
{
	std::string serverIp = "127.0.0.1";
	std::string serverPort = "12345";

	SOCKET clientSocket = Networking::createSocket(serverIp, serverPort);
	ASSERT_NE(clientSocket, INVALID_SOCKET) << "Failed to create client socket";
	std::unique_ptr<SOCKET, int (*)(SOCKET*)> lSocket(&clientSocket, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	bool connected = Networking::connectSocket(clientSocket, serverIp, serverPort, 5);
	EXPECT_FALSE(connected) << "Connection should have timed out";
}

struct ServerCommunication
{
	ServerCommunication()
	{
		WSADATA wsaData;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
		{
			throw std::runtime_error("WSAStartup failed");
		}

		m_serverIp = "127.0.0.1";
		m_serverPort = "44852";

		m_listeningSocket = Networking::createSocket(m_serverIp, m_serverPort);
		if (m_listeningSocket == INVALID_SOCKET)
		{
			throw std::runtime_error("Error creating socket");
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(stoi(m_serverPort));
		addr.sin_addr.s_addr = INADDR_ANY;
		result = bind(m_listeningSocket, (sockaddr*)&addr, sizeof(addr));
		if (result == SOCKET_ERROR)
		{
			closesocket(m_listeningSocket);
			throw std::runtime_error("Error binding socket");
		}

		result = listen(m_listeningSocket, SOMAXCONN);
		if (result == SOCKET_ERROR)
		{
			closesocket(m_listeningSocket);
			throw std::runtime_error("Error listening on socket");
		}

		m_emptyResponse.store(false);
		m_threadRunning.store(true);
		m_customResponse.store(nullptr);
		SOCKET sock = m_listeningSocket;
		m_serverThread = std::thread([sock, this]() {
			while (m_threadRunning.load(std::memory_order_relaxed))
			{
				SOCKET clientSocket = accept(sock, NULL, NULL);
				if (clientSocket != INVALID_SOCKET)
				{
					std::unique_ptr<SOCKET, int (*)(SOCKET*)> cSocket(&clientSocket, [](SOCKET* sck) {
						if (*sck != INVALID_SOCKET)
						{
							closesocket(*sck);
						}
						return 0;
					});

					char buf[1024]{};
					int len = recv(clientSocket, buf, 1024, 0);

					if (!m_emptyResponse.load())
					{
						auto response = m_customResponse.load();
						if (response == nullptr && len > 0)
						{
							send(clientSocket, buf, len, 0);
						}
						else
						{
							if (response != nullptr)
							{
								send(clientSocket, response, strlen(response), 0);
							}
						}
					}
				}
			}
		});
	}

	static SOCKET m_listeningSocket;
	static std::string m_serverPort;
	static std::string m_serverIp;
	static std::thread m_serverThread;
	static std::atomic<bool> m_emptyResponse;
	static std::atomic<const char*> m_customResponse;
	static std::atomic<bool> m_threadRunning;
	~ServerCommunication()
	{
		closesocket(m_listeningSocket);
		WSACleanup();
	}
};

std::string ServerCommunication::m_serverIp;
std::string ServerCommunication::m_serverPort;
std::thread ServerCommunication::m_serverThread;
std::atomic<bool> ServerCommunication::m_emptyResponse;
std::atomic<const char*> ServerCommunication::m_customResponse;
std::atomic<bool> ServerCommunication::m_threadRunning;
SOCKET ServerCommunication::m_listeningSocket;

static ServerCommunication s_server;

TEST(SendAllTest, SendSuccessfully)
{
	auto serverIp = ServerCommunication::m_serverIp;
	auto serverPort = ServerCommunication::m_serverPort;
	SOCKET clientSocket = Networking::createSocket(serverIp, serverPort);
	ASSERT_NE(clientSocket, INVALID_SOCKET);
	std::unique_ptr<SOCKET, int (*)(SOCKET*)> cSocket(&clientSocket, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	bool isConnected = Networking::connectSocket(clientSocket, serverIp, serverPort, 5000);
	ASSERT_TRUE(isConnected);

	const char* data = "Hello, world!";
	int readLength = 0;
	SOCKET result = Networking::sendAll(clientSocket, data, strlen(data), 0, &readLength);
	ASSERT_NE(result, INVALID_SOCKET);
	ASSERT_EQ(readLength, strlen(data));
}

TEST(SendAllTest, SendAllInvalidSocket)
{
	SOCKET sock = INVALID_SOCKET;
	const char* buffer = "test";
	int length = strlen(buffer);
	int flags = 0;

	int result = Networking::sendAll(sock, buffer, length, flags);

	EXPECT_EQ(SOCKET_ERROR, result);
}

TEST(SendAllTest, SendAllInvalidBuffer)
{
	auto serverIp = ServerCommunication::m_serverIp;
	auto serverPort = ServerCommunication::m_serverPort;
	SOCKET sock = Networking::createSocket(serverIp, serverPort);
	std::unique_ptr<SOCKET, int (*)(SOCKET*)> cSocket(&sock, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	const char* buffer = nullptr;
	int length = 5;
	int flags = 0;

	int result = Networking::sendAll(sock, buffer, length, flags);

	EXPECT_EQ(SOCKET_ERROR, result);
}

TEST(SendAllTest, SendAllInvalidLength)
{
	auto serverIp = ServerCommunication::m_serverIp;
	auto serverPort = ServerCommunication::m_serverPort;
	SOCKET sock = Networking::createSocket(serverIp, serverPort);
	std::unique_ptr<SOCKET, int (*)(SOCKET*)> cSocket(&sock, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	const char* buffer = "test";
	int length = -1;
	int flags = 0;

	int result = Networking::sendAll(sock, buffer, length, flags);

	EXPECT_EQ(SOCKET_ERROR, result);
}

TEST(SendAllTest, SendAllInvalidFlags)
{
	auto serverIp = ServerCommunication::m_serverIp;
	auto serverPort = ServerCommunication::m_serverPort;
	SOCKET sock = Networking::createSocket(serverIp, serverPort);
	std::unique_ptr<SOCKET, int (*)(SOCKET*)> cSocket(&sock, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	const char* buffer = "test";
	int length = strlen(buffer);
	int flags = -1;

	int result = Networking::sendAll(sock, buffer, length, flags);

	EXPECT_EQ(SOCKET_ERROR, result);
}

TEST(ReceiveResponseTest, ReceiveResponseSuccessfully)
{
	SOCKET clientSocket = Networking::createSocket(ServerCommunication::m_serverIp, ServerCommunication::m_serverPort);
	ASSERT_NE(clientSocket, INVALID_SOCKET);
	std::unique_ptr<SOCKET, int (*)(SOCKET*)> cSocket(&clientSocket, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	ASSERT_TRUE(Networking::connectSocket(clientSocket, ServerCommunication::m_serverIp, ServerCommunication::m_serverPort, 1000));

	const char* requestData = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
	int requestLength = strlen(requestData);
	int sentBytes = 0;
	auto result = Networking::sendAll(clientSocket, requestData, requestLength, 0, &sentBytes);
	ASSERT_NE(result, INVALID_SOCKET);
	ASSERT_EQ(sentBytes, requestLength);

	Networking::Response response;
	ASSERT_TRUE(Networking::receiveResponse(clientSocket, response));

	ASSERT_EQ(response.returnCode, 200);
	ASSERT_EQ(response.header["Content-Length"], "0");
	ASSERT_TRUE(response.body.empty());
}

TEST(ReceiveResponseTest, ReceiveResponseInvalidSocket)
{
	SOCKET clientSocket = Networking::createSocket(ServerCommunication::m_serverIp, ServerCommunication::m_serverPort);
	ASSERT_NE(clientSocket, INVALID_SOCKET);
	std::unique_ptr<SOCKET, int (*)(SOCKET*)> cSocket(&clientSocket, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	ASSERT_TRUE(Networking::connectSocket(clientSocket, ServerCommunication::m_serverIp, ServerCommunication::m_serverPort, 1000));

	const char* requestData = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
	int requestLength = strlen(requestData);
	int sentBytes = 0;
	auto result = Networking::sendAll(clientSocket, requestData, requestLength, 0, &sentBytes);
	ASSERT_NE(result, INVALID_SOCKET);
	ASSERT_EQ(sentBytes, requestLength);

	Networking::Response response;
	EXPECT_FALSE(Networking::receiveResponse(INVALID_SOCKET, response));
}

TEST(ReceiveResponseTest, ReceiveResponseIncorrectResponse)
{
	SOCKET clientSocket = Networking::createSocket(ServerCommunication::m_serverIp, ServerCommunication::m_serverPort);
	ASSERT_NE(clientSocket, INVALID_SOCKET);
	std::unique_ptr<SOCKET, int (*)(SOCKET*)> cSocket(&clientSocket, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	ASSERT_TRUE(Networking::connectSocket(clientSocket, ServerCommunication::m_serverIp, ServerCommunication::m_serverPort, 1000));

	const char* requestData = "HTTP/1.1 200 OK\r\nBodyAndHeader";
	int requestLength = strlen(requestData);
	int sentBytes = 0;
	auto result = Networking::sendAll(clientSocket, requestData, requestLength, 0, &sentBytes);
	ASSERT_NE(result, INVALID_SOCKET);
	ASSERT_EQ(sentBytes, requestLength);

	Networking::Response response;
	ASSERT_FALSE(Networking::receiveResponse(clientSocket, response));

	ASSERT_EQ(response.returnCode, 200);
	ASSERT_TRUE(response.header.empty());
	ASSERT_TRUE(response.body.empty());
}

TEST(ReceiveResponseTest, ReceiveResponseEmptyResponse)
{
	SOCKET clientSocket = Networking::createSocket(ServerCommunication::m_serverIp, ServerCommunication::m_serverPort);
	ASSERT_NE(clientSocket, INVALID_SOCKET);
	std::unique_ptr<SOCKET, int (*)(SOCKET*)> cSocket(&clientSocket, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	ASSERT_TRUE(Networking::connectSocket(clientSocket, ServerCommunication::m_serverIp, ServerCommunication::m_serverPort, 1000));

	ServerCommunication::m_emptyResponse.store(true);
	const char* requestData = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
	int requestLength = strlen(requestData);
	int sentBytes = 0;
	auto result = Networking::sendAll(clientSocket, requestData, requestLength, 0, &sentBytes);
	ASSERT_NE(result, INVALID_SOCKET);
	ASSERT_EQ(sentBytes, requestLength);

	Networking::Response response;
	ASSERT_FALSE(Networking::receiveResponse(clientSocket, response));

	ASSERT_EQ(response.returnCode, 0);
	ASSERT_TRUE(response.header.empty());
	ASSERT_TRUE(response.body.empty());

	ServerCommunication::m_emptyResponse.store(false);
}

TEST(SendRequestTest, SendRequestSuccessfully)
{
	SOCKET clientSocket = Networking::createSocket(ServerCommunication::m_serverIp, ServerCommunication::m_serverPort);
	ASSERT_NE(clientSocket, INVALID_SOCKET);
	ASSERT_TRUE(Networking::connectSocket(clientSocket, ServerCommunication::m_serverIp, ServerCommunication::m_serverPort, 1000));

	std::string url = "http://" + ServerCommunication::m_serverIp + ":" + ServerCommunication::m_serverPort;
	std::unordered_map<std::string, std::string> headers = {
		 {"User-Agent", "MyApp/1.0"},
		 {"Accept-Language", "en-US"},
	};
	json body = {
		 {"foo", 123},
		 {"bar", "hello"},
	};

	Networking::Response response;
	ServerCommunication::m_customResponse.store("HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\n{\"admin\":\"admin\"}");

	bool result = Networking::send_request(clientSocket, url, headers, body, response, true);
	ServerCommunication::m_customResponse.store(nullptr);

	ASSERT_TRUE(result);

	auto lengthString = response.header["Content-Length"].dump();
	auto length = atoi(lengthString.data() + 1);
	ASSERT_EQ(response.returnCode, 200);
	ASSERT_EQ(length, 12);
	ASSERT_EQ(response.body["admin"], "admin");

	closesocket(clientSocket);
}

TEST(SendRequestTest, SendRequestSuccessffullyResponse)
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		FAIL() << "WSAStartup failed";
	}

	std::string ip = "127.0.0.1";
	std::string port = "62345";

	SOCKET listenSocket = Networking::createSocket(ip, port);
	if (listenSocket == INVALID_SOCKET)
	{
		FAIL() << "Error creating socket";
	}

	std::unique_ptr<SOCKET, int (*)(SOCKET*)> lSocket(&listenSocket, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(stoi(port));
	addr.sin_addr.s_addr = INADDR_ANY;
	result = bind(listenSocket, (sockaddr*)&addr, sizeof(addr));
	if (result == SOCKET_ERROR)
	{
		FAIL() << "Error binding socket";
	}

	result = listen(listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR)
	{
		FAIL() << "Error listening on socket";
	}

	auto responseResult = std::async([listenSocket]() {
		SOCKET clientConnectionSocket = accept(listenSocket, NULL, NULL);

		char buf[1024]{};
		int len = recv(clientConnectionSocket, buf, 1024, 0);
		std::string responseMessage(buf, len);
		closesocket(clientConnectionSocket);

		return responseMessage;
	});

	SOCKET clientSocket = Networking::createSocket(ip, port);
	ASSERT_NE(clientSocket, INVALID_SOCKET);
	std::unique_ptr<SOCKET, int (*)(SOCKET*)> cSocket(&clientSocket, [](SOCKET* sck) {
		if (*sck != INVALID_SOCKET)
		{
			closesocket(*sck);
		}
		return 0;
	});

	ASSERT_TRUE(Networking::connectSocket(clientSocket, ip, port, 1000));

	std::string url = "http://" + ip + ":" + port;
	std::unordered_map<std::string, std::string> headers = {
		 {"User-Agent", "MyApp/1.0"},
		 {"Accept-Language", "en-US"},
	};

	json body = {
		 {"foo", 123},
		 {"bar", "hello"},
	};

	Networking::Response response;
	EXPECT_FALSE(Networking::send_request(clientSocket, url, headers, body, response, true));

	auto responseMessage = std::move(responseResult.get());
	EXPECT_NE(responseMessage.find("POST"), std::string::npos);
	EXPECT_NE(responseMessage.find("User-Agent: MyApp/1.0"), std::string::npos);
	EXPECT_NE(responseMessage.find("Accept-Language: en-US"), std::string::npos);
	EXPECT_NE(responseMessage.find("\"foo\":123"), std::string::npos);
	EXPECT_NE(responseMessage.find("\"bar\":\"hello\""), std::string::npos);

	WSACleanup();
}

TEST(SendRequestTest, SendRequestInvalidSocket)
{
	// Create a client socket and connect it to the server
	SOCKET clientSocket = Networking::createSocket(ServerCommunication::m_serverIp, ServerCommunication::m_serverPort);
	ASSERT_NE(clientSocket, INVALID_SOCKET);
	ASSERT_TRUE(Networking::connectSocket(clientSocket, ServerCommunication::m_serverIp, ServerCommunication::m_serverPort, 1000));

	std::string url = "http://" + ServerCommunication::m_serverIp + ":" + ServerCommunication::m_serverPort;
	std::unordered_map<std::string, std::string> headers = {
		 {"User-Agent", "MyApp/1.0"},
		 {"Accept-Language", "en-US"},
	};
	json body = {
		 {"foo", 123},
		 {"bar", "hello"},
	};

	Networking::Response response;
	bool result = Networking::send_request(INVALID_SOCKET, url, headers, body, response, true);
	EXPECT_FALSE(result);
}

TEST(formMediaServerApiTests, TestInvalidCredentials)
{
	std::string imageName = "AppIcon.png";
	std::string testFile = fmt::format("resources/icons/{}", imageName);
	DataGraph::Networking::ConnectionData auth;
	std::string extension = imageName.substr(imageName.find_first_of('.'));
	auth.ip = "127.0.0.1";
	auth.port = "8082";
	auth.login = "admin";
	auth.password = "invalid";
	DataGraph::Networking::Response response;
	DataGraph::Networking::connectToMediaServer(auth, response);
	EXPECT_EQ(response.returnCode, 401);
}

TEST(formMediaServerApiTests, TestSuccessfulAuthentification)
{
	std::string imageName = "AppIcon.png";
	std::string testFile = fmt::format("resources/icons/{}", imageName);
	DataGraph::Networking::ConnectionData auth;
	std::string extension = imageName.substr(imageName.find_first_of('.'));
	auth.ip = "127.0.0.1";
	auth.port = "8082";
	auth.login = "admin";
	auth.password = "example";
	DataGraph::Networking::Response response;
	DataGraph::Networking::connectToMediaServer(auth, response);
	EXPECT_EQ(response.returnCode, 200);
}

TEST(formMediaServerApiTests, TestUploadMediaServer)
{
	std::string imageName = "AppIcon.png";
	std::string testFile = fmt::format("resources/icons/{}", imageName);
	DataGraph::Networking::ConnectionData auth;
	std::string extension = imageName.substr(imageName.find_first_of('.'));
	auth.ip = "127.0.0.1";
	auth.port = "8082";
	auth.login = "admin";
	auth.password = "example";
	DataGraph::Networking::Response response;
	DataGraph::Networking::loadFileToHost(testFile, auth, response);

	ASSERT_TRUE(response.body.contains("fileName"));
	auto fileName = response.body["fileName"].get<std::string_view>();
	EXPECT_EQ(fileName, utils::GetMD5String(imageName) + extension);
}

TEST(formMediaServerApiTests, TestUploadExistsMediaServer)
{
	std::string imageName = "AppIcon.png";
	std::string testFile = fmt::format("resources/icons/{}", imageName);
	DataGraph::Networking::ConnectionData auth;
	std::string extension = imageName.substr(imageName.find_first_of('.'));
	auth.ip = "127.0.0.1";
	auth.port = "8082";
	auth.login = "admin";
	auth.password = "example";
	DataGraph::Networking::Response response;
	DataGraph::Networking::loadFileToHost(testFile, auth, response);
	EXPECT_TRUE(response.returnCode, 400);
	EXPECT_TRUE(response.body.contains("error"));

	ASSERT_TRUE(response.body.contains("fileName"));
	auto fileName = response.body["fileName"].get<std::string_view>();
	EXPECT_EQ(fileName, utils::GetMD5String(imageName) + extension);
}

TEST(formDatabaseConnection, SuccessfulConnection)
{
	std::string ip = "127.0.0.1";
	std::string port = "5432";
	std::string login = "postgres";
	std::string password = "example";

	dmitigr::pgfe::Connection_options options;
	options.set(dmitigr::pgfe::Communication_mode::net)
		 .set_hostname(ip)
		 .set_port(std::stoi(port))
		 .set_database("DataGraph")
		 .set_username(login)
		 .set_password(password);

	dmitigr::pgfe::Connection conn(options);
	EXPECT_NO_THROW(conn.connect());
	EXPECT_TRUE(conn.is_connected());
	conn.disconnect();
}

TEST(formDatabaseConnection, InvalidConnection)
{
	std::string ip = "127.0.0.1";
	std::string port = "5432";
	std::string login = "postgres";
	std::string password = "invalid";

	dmitigr::pgfe::Connection_options options;
	options.set(dmitigr::pgfe::Communication_mode::net)
		 .set_hostname(ip)
		 .set_port(std::stoi(port))
		 .set_database("DataGraph")
		 .set_username(login)
		 .set_password(password);

	dmitigr::pgfe::Connection conn(options);

	EXPECT_ANY_THROW(conn.connect());
	EXPECT_FALSE(conn.is_connected());
}
