#include "Unit-tests.hpp"

#include <Networking/backend/Connection.hpp>

namespace DataGraph::tests
{
std::string ConnectionServer::m_serverIp;
std::string ConnectionServer::m_serverPort;
std::atomic<bool> ConnectionServer::m_emptyResponse;
std::atomic<const char*> ConnectionServer::m_customResponse;

ConnectionServer::ConnectionServer(std::string port) { m_serverPort = std::move(port); }

void ConnectionServer::SetUp()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		throw std::runtime_error("WSAStartup has failed");
	}

	m_serverIp = "127.0.0.1";

	m_listeningSocket = Networking::backend::createSocket(m_serverIp, m_serverPort);
	if (&m_listeningSocket == INVALID_SOCKET)
	{
		throw std::runtime_error("Error creating socket");
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(stoi(m_serverPort));
	addr.sin_addr.s_addr = INADDR_ANY;
	result = bind(&m_listeningSocket, (sockaddr*)&addr, sizeof(addr));
	if (result == SOCKET_ERROR)
	{
		throw std::runtime_error("Error binding socket");
	}

	result = listen(&m_listeningSocket, SOMAXCONN);
	if (result == SOCKET_ERROR)
	{
		throw std::runtime_error("Error listening on socket");
	}

	m_emptyResponse.store(false);
	m_threadRunning.store(true);
	m_customResponse.store(nullptr);

	m_thread = std::thread(
		 [&](Networking::backend::SocketWrapper& sckt, std::atomic<const char*>& customResponse, std::atomic_bool& isEmpty) {
			 while (m_threadRunning.load())
			 {
				 Networking::backend::SocketWrapper clientSocket(accept(&sckt, NULL, NULL));
				 if (&clientSocket == INVALID_SOCKET)
				 {
					 break;
				 }

				 constexpr const uint16_t bufferSize = 1024;
				 char buf[bufferSize]{};
				 int len = recv(&clientSocket, buf, bufferSize, 0);
				 if (!isEmpty)
				 {
					 auto response = customResponse.load();
					 if (response == nullptr && len > 0)
					 {
						 send(&clientSocket, buf, len, 0);
					 }
					 else
					 {
						 if (response != nullptr)
						 {
							 send(&clientSocket, response, strlen(response), 0);
						 }
					 }
				 }
			 }
		 },
		 std::ref(m_listeningSocket), std::ref(m_customResponse), std::ref(m_emptyResponse));
}

void ConnectionServer::TearDown()
{
	m_listeningSocket.~SocketWrapper();
	m_threadRunning.store(false);
	if (m_thread.joinable())
	{
		m_thread.join();
	}
	WSACleanup();
}

const std::string_view ConnectionServer::getIp() { return m_serverIp; }

const std::string_view ConnectionServer::getPort() { return m_serverPort; }
}  // namespace DataGraph::tests
