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

#include <stb_image.h>
#include <utils/Encrypt.hpp>

#include <pgfe/pgfe.hpp>

#include <Networking/backend/Connection.hpp>

#include "Unit-tests.hpp"

using namespace DataGraph;
using namespace nlohmann;

TEST(CreateHeadersStringTest, EmptyHeaders)
{
	std::unordered_map<std::string, std::string> headers;
	EXPECT_EQ(Networking::backend::createHeadersString(headers), "");
}

TEST(CreateHeadersStringTest, SingleHeader)
{
	std::unordered_map<std::string, std::string> headers = {{"Content-Type", "text/plain"}};
	EXPECT_EQ(Networking::backend::createHeadersString(headers), "Content-Type: text/plain\r\n");
}

TEST(CreateHeadersStringTest, MultipleHeaders)
{
	std::unordered_map<std::string, std::string> headers
		 = {{"Content-Type", "text/html"}, {"Accept-Language", "en-US"}, {"Authorization", "Bearer abcdef1234567890"}};

	std::string result = Networking::backend::createHeadersString(headers);
	EXPECT_EQ(result, "Content-Type: text/html\r\nAuthorization: Bearer abcdef1234567890\r\nAccept-Language: en-US\r\n");
}

TEST(ParseHeader, ValidHeader)
{
	std::string contentType = "application/json";
	std::string authorization = "Bearer abc123";
	std::string header = fmt::format("Content-Type: {}\r\nAuthorization: {}\r\n", contentType, authorization);

	json result;
	auto errorCode = Networking::backend::parseHeader(header, result);
	ASSERT_TRUE(Networking::backend::isOK(errorCode));
	ASSERT_TRUE(result.contains("Content-Type"));
	ASSERT_TRUE(result.contains("Authorization"));
	EXPECT_EQ(result["Content-Type"], contentType);
	EXPECT_EQ(result["Authorization"], authorization);
}

TEST(ParseHeader, MalformedHeaderMissingColon)
{
	std::string_view header = "Content-Type: application/json\r\nAuthorization Bearer abc123\r\n";

	json result;
	auto errorCode = Networking::backend::parseHeader(header, result);
	EXPECT_EQ(errorCode, Networking::backend::ErrorCode::NO_VALUE_SEPARATOR_FOUND);
}

TEST(ParseHeader, MalformedHeaderMissingSeparator)
{
	std::string_view header = "Content-Type: application/jsonAuthorization: Bearer abc123\r\n";

	json result;
	auto errorCode = Networking::backend::parseHeader(header, result);
	EXPECT_TRUE(Networking::backend::isOK(errorCode));
	EXPECT_FALSE(result.contains("Authorization"));
}

TEST(ParseHeader, EmptyHeader)
{
	std::string_view header = "";
	json result;
	auto errorCode = Networking::backend::parseHeader(header, result);
	EXPECT_TRUE(Networking::backend::isOK(errorCode));
	EXPECT_TRUE(result.empty());
}

TEST(CreateSocketTest, ValidAddressAndPort)
{
	const std::string_view address = tests::ConnectionServer::getIp();
	const std::string_view port = tests::ConnectionServer::getPort();

	Networking::backend::ErrorCodes errorStatus{};
	auto sock = Networking::backend::createSocket(address, port, &errorStatus);

	EXPECT_TRUE(Networking::backend::isOK(errorStatus));
	EXPECT_NE(&sock, INVALID_SOCKET);
}

TEST(CreateSocketTest, InvalidAddress)
{
	const std::string_view invalidAddress = "invalidAddress";
	const std::string_view validPort = tests::ConnectionServer::getPort();

	Networking::backend::ErrorCodes errorStatus{};
	auto socket = Networking::backend::createSocket(invalidAddress, validPort, &errorStatus);

	EXPECT_FALSE(Networking::backend::isOK(errorStatus));
	EXPECT_EQ(&socket, INVALID_SOCKET);
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

	std::string getPath(const std::string& path) { return m_testDir.string() + '\\' + path; }

private:
	std::filesystem::path m_testDir;
};

TEST_F(ReadFileTest, NonexistentFile)
{
	std::vector<char> fileContents;
	EXPECT_EQ(Networking::backend::ErrorCode::FILE_NOT_FOUND,
		 Networking::backend::readFile(ReadFileTest::getPath("nonexistent_file.txt"), fileContents));
}

TEST_F(ReadFileTest, EmptyFile)
{
	auto filePath = getPath("EmptyFile.txt");
	std::ofstream file(filePath);
	file.close();

	std::vector<char> fileContents;
	EXPECT_EQ(Networking::backend::ErrorCode::OK, Networking::backend::readFile(filePath, fileContents));
	EXPECT_EQ(0, fileContents.size());
}

TEST_F(ReadFileTest, SmallFile)
{
	auto filePath = getPath("SmallFile.txt");
	std::ofstream file(filePath);

	std::string fileData = "THIS IS TEST MESSAGE!";
	file << fileData;
	file.close();

	std::vector<char> fileContents;
	EXPECT_EQ(Networking::backend::ErrorCode::OK, Networking::backend::readFile(filePath, fileContents));
	EXPECT_EQ(fileData.size(), fileContents.size());
}

TEST_F(ReadFileTest, LargeFile)
{
	auto filePath = getPath("LargeFile.txt");
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
	EXPECT_EQ(Networking::backend::ErrorCode::OK, Networking::backend::readFile(filePath, fileContents));
	EXPECT_EQ(size, fileContents.size());
}

TEST(getContentTypeTest, RandomDotsTest)
{
	std::string_view type = "GSDFGS.GSDFGS.SDFGSDFG.txt";
	std::string_view expect = "text/plain";

	auto result = Networking::backend::getContentType(type);
	EXPECT_EQ(result, expect);
}

TEST(getContentTypeTest, EmptyExtension)
{
	std::string_view type = "GSDFGS";
	std::string_view expect = "application/octet-stream";

	auto result = Networking::backend::getContentType(type);
	EXPECT_EQ(result, expect);
}

TEST(getContentTypeTest, EmptyType)
{
	std::string_view type = "";
	std::string_view expect = "";

	auto result = Networking::backend::getContentType(type);
	EXPECT_EQ(result, expect);
}

TEST(ConnectSocketTest, connectionSuccessfully)
{
	Networking::backend::ErrorCodes errorCode{};
	auto serverIp = tests::ConnectionServer::getIp();
	auto serverPort = tests::ConnectionServer::getPort();

	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort, &errorCode);
	ASSERT_TRUE(Networking::backend::isOK(errorCode));
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	auto result = Networking::backend::connectSocket(clientSocket, serverIp, serverPort, 5);
	EXPECT_EQ(result, Networking::backend::ErrorCode::OK);
}

TEST(ConnectSocketTest, FailsToConnect)
{
	const std::string_view address = "invalidAddress";
	const std::string_view port = "12345";

	Networking::backend::ErrorCodes errorCode{};
	auto testSocket = Networking::backend::createSocket(address, port, &errorCode);

	auto result = Networking::backend::connectSocket(testSocket, address, port, 5);
	EXPECT_EQ(result, Networking::backend::ErrorCode::INVALID_PARAMS);
}

TEST(ConnectSocketTest, FailsToConnectSocket)
{
	const std::string_view address = "invalidAddress";
	const std::string_view port = "12345";

	Networking::backend::SocketWrapper testSocket;
	auto result = Networking::backend::connectSocket(testSocket, address, port, 5);

	EXPECT_EQ(result, Networking::backend::ErrorCode::INVALID_PARAMS);
}

TEST(ConnectSocketTest, ConnectionTimeout)
{
	std::string serverIp = "127.0.0.1";
	std::string serverPort = "1";

	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort);
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	Networking::backend::ErrorCodes result = Networking::backend::connectSocket(clientSocket, serverIp, serverPort, 5);
	EXPECT_TRUE(result == Networking::backend::ErrorCode::CONNECTION_TIMEOUT || result == Networking::backend::ErrorCode::CONNECTION_FAILED)
		 << "Connection should have timed out";
}

// Quite easily could be replaced with parametrized tests, but it's not necessary
TEST(SendAllTest, Successfully)
{
	auto serverIp = tests::ConnectionServer::getIp();
	auto serverPort = tests::ConnectionServer::getPort();
	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort);
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	Networking::backend::ErrorCodes isConnected = Networking::backend::connectSocket(clientSocket, serverIp, serverPort, 5000);
	ASSERT_TRUE(Networking::backend::isOK(isConnected));

	const char* data = "Hello, world!";
	int readLength = 0;
	Networking::backend::ErrorCodes result = Networking::backend::sendAll(clientSocket, data, strlen(data), 0, &readLength);
	EXPECT_TRUE(Networking::backend::isOK(result));
	EXPECT_EQ(readLength, strlen(data));
}

TEST(SendAllTest, InvalidSocket)
{
	Networking::backend::SocketWrapper sock;
	const char* buffer = "test";
	int length = strlen(buffer);
	int flags = 0;

	Networking::backend::ErrorCodes result = Networking::backend::sendAll(sock, buffer, length, flags);
	EXPECT_EQ(result, Networking::backend::ErrorCode::INVALID_PARAMS);
}

TEST(SendAllTest, InvalidSendingBuffer)
{
	auto serverIp = tests::ConnectionServer::getIp();
	auto serverPort = tests::ConnectionServer::getPort();

	Networking::backend::ErrorCodes errorCode;
	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort, &errorCode);
	ASSERT_TRUE(Networking::backend::isOK(errorCode));
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	const char* buffer = nullptr;
	int length = 5;
	int flags = 0;

	Networking::backend::ErrorCodes result = Networking::backend::sendAll(clientSocket, buffer, length, flags);
	EXPECT_EQ(Networking::backend::ErrorCode::INVALID_PARAMS, result);
}

TEST(SendAllTest, InvalidLength)
{
	auto serverIp = tests::ConnectionServer::getIp();
	auto serverPort = tests::ConnectionServer::getPort();

	Networking::backend::ErrorCodes errorCode;
	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort, &errorCode);
	ASSERT_TRUE(Networking::backend::isOK(errorCode));
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	const char* buffer = "test";
	int length = -1;
	int flags = 0;

	Networking::backend::ErrorCodes result = Networking::backend::sendAll(clientSocket, buffer, length, flags);
	EXPECT_EQ(Networking::backend::ErrorCode::INVALID_PARAMS, result);
}

TEST(SendAllTest, InvalidFlags)
{
	auto serverIp = tests::ConnectionServer::getIp();
	auto serverPort = tests::ConnectionServer::getPort();
	Networking::backend::ErrorCodes errorCode;
	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort, &errorCode);
	ASSERT_TRUE(Networking::backend::isOK(errorCode));
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	const char* buffer = "test";
	int length = strlen(buffer);
	int flags = -1;

	Networking::backend::ErrorCodes result = Networking::backend::sendAll(clientSocket, buffer, length, flags);
	EXPECT_EQ(Networking::backend::ErrorCode::SEND_FAILED, result);
}

TEST(ReceiveResponseTest, Successfully)
{
	auto serverIp = tests::ConnectionServer::getIp();
	auto serverPort = tests::ConnectionServer::getPort();

	Networking::backend::ErrorCodes errorCode;
	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort, &errorCode);
	ASSERT_TRUE(Networking::backend::isOK(errorCode));
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	auto result = Networking::backend::connectSocket(clientSocket, serverIp, serverPort, 1000);
	ASSERT_TRUE(Networking::backend::isOK(result));

	const char* requestData = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
	int requestLength = strlen(requestData);
	int sentBytes = 0;
	result = Networking::backend::sendAll(clientSocket, requestData, requestLength, 0, &sentBytes);
	ASSERT_TRUE(Networking::backend::isOK(result));
	ASSERT_EQ(sentBytes, requestLength);

	Networking::Response response;
	ASSERT_TRUE(Networking::backend::isOK(Networking::backend::receiveResponse(clientSocket, response)));

	EXPECT_EQ(response.returnCode, 200);
	EXPECT_EQ(response.header["Content-Length"], "0");
	EXPECT_TRUE(response.body.empty());
}

TEST(ReceiveResponseTest, InvalidSocket)
{
	Networking::backend::ErrorCodes errorCode;
	Networking::backend::SocketWrapper clientSocket;

	const char* requestData = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
	int requestLength = strlen(requestData);
	int sentBytes = 0;
	auto result = Networking::backend::sendAll(clientSocket, requestData, requestLength, 0, &sentBytes);
	ASSERT_EQ(result, Networking::backend::ErrorCode::INVALID_PARAMS);
	ASSERT_EQ(sentBytes, 0);

	Networking::Response response;
	EXPECT_EQ(Networking::backend::receiveResponse(clientSocket, response), Networking::backend::ErrorCode::RECEIVE_ERROR);
}

TEST(ReceiveResponseTest, ReceiveResponseIncorrectResponse)
{
	auto serverIp = tests::ConnectionServer::getIp();
	auto serverPort = tests::ConnectionServer::getPort();

	Networking::backend::ErrorCodes errorCode;
	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort, &errorCode);
	ASSERT_TRUE(Networking::backend::isOK(errorCode));
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	auto result = Networking::backend::connectSocket(clientSocket, serverIp, serverPort, 1000);
	ASSERT_TRUE(Networking::backend::isOK(result));

	const char* requestData = "HTTP/1.1 200 OK\r\nBodyAndHeader";
	int requestLength = strlen(requestData);
	int sentBytes = 0;
	result = Networking::backend::sendAll(clientSocket, requestData, requestLength, 0, &sentBytes);
	EXPECT_TRUE(Networking::backend::isOK(result));
	EXPECT_EQ(sentBytes, strlen(requestData));

	Networking::Response response;
	result = Networking::backend::receiveResponse(clientSocket, response);

	EXPECT_EQ(response.returnCode, 200);
	EXPECT_TRUE(response.header.empty());
	EXPECT_TRUE(response.body.empty());
}

TEST(ReceiveResponseTest, EmptyResponse)
{
	auto serverIp = tests::ConnectionServer::getIp();
	auto serverPort = tests::ConnectionServer::getPort();

	Networking::backend::ErrorCodes errorCode;
	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort, &errorCode);
	ASSERT_TRUE(Networking::backend::isOK(errorCode));
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	auto result
		 = Networking::backend::connectSocket(clientSocket, tests::ConnectionServer::getIp(), tests::ConnectionServer::getPort(), 1000);
	ASSERT_TRUE(Networking::backend::isOK(result));

	tests::ConnectionServer::m_emptyResponse.store(true);
	const char* requestData = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
	int requestLength = strlen(requestData);
	int sentBytes = 0;
	result = Networking::backend::sendAll(clientSocket, requestData, requestLength, 0, &sentBytes);
	ASSERT_NE(result, INVALID_SOCKET);
	ASSERT_EQ(sentBytes, requestLength);

	Networking::Response response;
	result = Networking::backend::receiveResponse(clientSocket, response);
	EXPECT_EQ(result, Networking::backend::ErrorCode::CONNECTION_CLOSED);

	EXPECT_EQ(response.returnCode, 0);
	EXPECT_TRUE(response.header.empty());
	EXPECT_TRUE(response.body.empty());

	tests::ConnectionServer::m_emptyResponse.store(false);
}

TEST(SendRequestTest, SendRequestSuccessfully)
{
	auto serverIp = tests::ConnectionServer::getIp();
	auto serverPort = tests::ConnectionServer::getPort();

	Networking::backend::ErrorCodes errorCode;
	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort, &errorCode);
	ASSERT_TRUE(Networking::backend::isOK(errorCode));
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	auto result
		 = Networking::backend::connectSocket(clientSocket, tests::ConnectionServer::getIp(), tests::ConnectionServer::getPort(), 1000);
	ASSERT_TRUE(Networking::backend::isOK(result));

	std::string url = fmt::format("http://{}:{}", tests::ConnectionServer::getIp(), tests::ConnectionServer::getPort());
	std::unordered_map<std::string, std::string> headers = {
		 {"User-Agent", "MyApp/1.0"},
		 {"Accept-Language", "en-US"},
	};
	json body = {
		 {"foo", 123},
		 {"bar", "hello"},
	};

	Networking::Response response;
	tests::ConnectionServer::m_customResponse.store("HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\n{\"admin\":\"admin\"}");

	result = Networking::backend::sendRequest(clientSocket, url, headers, body, response, true);
	tests::ConnectionServer::m_customResponse.store(nullptr);
	ASSERT_TRUE(Networking::backend::isOK(result));

	auto lengthString = response.header["Content-Length"].dump();
	auto length = atoi(lengthString.data() + 1);
	EXPECT_EQ(response.returnCode, 200);
	EXPECT_EQ(length, 12);
	EXPECT_EQ(response.body["admin"], "admin");
}


TEST(SendRequestTest, SendRequestSuccessffullyResponse)
{
	auto serverIp = tests::ConnectionServer::getIp();
	auto serverPort = tests::ConnectionServer::getPort();

	Networking::backend::ErrorCodes errorCode;
	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort, &errorCode);
	ASSERT_TRUE(Networking::backend::isOK(errorCode));
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	auto result
		 = Networking::backend::connectSocket(clientSocket, tests::ConnectionServer::getIp(), tests::ConnectionServer::getPort(), 1000);
	ASSERT_TRUE(Networking::backend::isOK(result));

	std::string url = fmt::format("http://{}:{}", tests::ConnectionServer::getIp(), tests::ConnectionServer::getPort());
	std::unordered_map<std::string, std::string> headers = {
		 {"User-Agent", "MyApp/1.0"},
		 {"Accept-Language", "en-US"},
	};

	json body = {
		 {"foo", 123},
		 {"bar", "hello"},
	};

	Networking::Response response;
	result = Networking::backend::sendRequest(clientSocket, url, headers, body, response, true);
	ASSERT_TRUE(Networking::backend::isOK(result));

	auto& responseMessage = response.raw;
	EXPECT_NE(responseMessage.find("POST"), std::string::npos);
	EXPECT_NE(responseMessage.find("User-Agent: MyApp/1.0"), std::string::npos);
	EXPECT_NE(responseMessage.find("Accept-Language: en-US"), std::string::npos);
	EXPECT_NE(responseMessage.find("\"foo\":123"), std::string::npos);
	EXPECT_NE(responseMessage.find("\"bar\":\"hello\""), std::string::npos);
}

TEST(SendRequestTest, SendRequestInvalidSocket)
{
	auto serverIp = tests::ConnectionServer::getIp();
	auto serverPort = tests::ConnectionServer::getPort();

	Networking::backend::ErrorCodes errorCode;
	auto clientSocket = Networking::backend::createSocket(serverIp, serverPort, &errorCode);
	ASSERT_TRUE(Networking::backend::isOK(errorCode));
	ASSERT_NE(&clientSocket, INVALID_SOCKET);

	auto result
		 = Networking::backend::connectSocket(clientSocket, tests::ConnectionServer::getIp(), tests::ConnectionServer::getPort(), 1000);
	ASSERT_TRUE(Networking::backend::isOK(result));

	std::string url = fmt::format("http://{}:{}", tests::ConnectionServer::getIp(), tests::ConnectionServer::getPort());
	std::unordered_map<std::string, std::string> headers = {
		 {"User-Agent", "MyApp/1.0"},
		 {"Accept-Language", "en-US"},
	};
	json body = {
		 {"foo", 123},
		 {"bar", "hello"},
	};

	Networking::Response response;
	Networking::backend::SocketWrapper sock;
	result = Networking::backend::sendRequest(sock, url, headers, body, response, true);
	EXPECT_FALSE(Networking::backend::isOK(result));
}

#include "integration.hpp"

// In future auth params could be moved to config or args/ENV
TEST_INTEGRATED(formMediaServerApiTests, TestInvalidCredentials)
{
	Networking::ConnectionData auth;
	auth.ip = "127.0.0.1";
	auth.port = "8082";
	auth.login = "admin";
	auth.password = "invalid";

	Networking::Response response;
	ASSERT_TRUE(Networking::backend::isOK(Networking::backend::connectToMediaServer(auth, response)));
	EXPECT_EQ(response.returnCode, 401);
}

TEST_INTEGRATED(formMediaServerApiTests, TestSuccessfulAuthentification)
{
	std::string imageName = "AppIcon.png";
	std::string testFile = fmt::format("resources/icons/{}", imageName);
	Networking::ConnectionData auth;
	std::string extension = imageName.substr(imageName.find_first_of('.'));
	auth.ip = "127.0.0.1";
	auth.port = "8082";
	auth.login = "admin";
	auth.password = "example";

	Networking::Response response;
	EXPECT_TRUE(Networking::backend::isOK(Networking::backend::connectToMediaServer(auth, response)));
	EXPECT_EQ(response.returnCode, 200);
}

TEST_INTEGRATED(formMediaServerApiTests, TestUploadMediaServer)
{
	std::string imageName = "AppIcon.png";
	std::string testFile = fmt::format("resources/icons/{}", imageName);
	Networking::ConnectionData auth;
	std::string extension = imageName.substr(imageName.find_first_of('.'));
	auth.ip = "127.0.0.1";
	auth.port = "8082";
	auth.login = "admin";
	auth.password = "example";
	Networking::Response response;
	Networking::backend::loadFileToHost(testFile, auth, response);

	ASSERT_TRUE(response.body.contains("fileName"));
	auto fileName = response.body["fileName"].get<std::string_view>();
	EXPECT_EQ(fileName, utils::GetMD5String(imageName) + extension);
}

TEST_INTEGRATED(formMediaServerApiTests, TestUploadExistsMediaServer)
{
	std::string imageName = "AppIcon.png";
	std::string testFile = fmt::format("resources/icons/{}", imageName);
	Networking::ConnectionData auth;
	std::string extension = imageName.substr(imageName.find_first_of('.'));
	auth.ip = "127.0.0.1";
	auth.port = "8082";
	auth.login = "admin";
	auth.password = "example";
	Networking::Response response;
	EXPECT_TRUE(Networking::backend::isOK(Networking::backend::loadFileToHost(testFile, auth, response)));
	EXPECT_TRUE(response.returnCode, 400);
	EXPECT_TRUE(response.body.contains("error"));

	ASSERT_TRUE(response.body.contains("fileName"));
	auto fileName = response.body["fileName"].get<std::string_view>();
	EXPECT_EQ(fileName, utils::GetMD5String(imageName) + extension);
}

TEST_INTEGRATED(formDatabaseConnection, SuccessfulConnection)
{
	std::string ip = "127.0.0.1";
	std::string port = "5435";
	std::string login = "postgres";
	std::string password = "example";

	dmitigr::pgfe::Connection_options options;
	options.set(dmitigr::pgfe::Communication_mode::net)
		 .set_hostname(ip)
		 .set_port(std::stoi(port))
		 .set_database("datagraph")
		 .set_username(login)
		 .set_password(password);

	dmitigr::pgfe::Connection conn(options);
	EXPECT_NO_THROW(conn.connect());
	EXPECT_TRUE(conn.is_connected());
	conn.disconnect();
}

TEST_INTEGRATED(formDatabaseConnection, InvalidConnection)
{
	std::string ip = "127.0.0.1";
	std::string port = "5435";
	std::string login = "postgres";
	std::string password = "invalid";

	dmitigr::pgfe::Connection_options options;
	options.set(dmitigr::pgfe::Communication_mode::net)
		 .set_hostname(ip)
		 .set_port(std::stoi(port))
		 .set_database("datagraph")
		 .set_username(login)
		 .set_password(password);

	dmitigr::pgfe::Connection conn(options);

	EXPECT_ANY_THROW(conn.connect());
	EXPECT_FALSE(conn.is_connected());
}
