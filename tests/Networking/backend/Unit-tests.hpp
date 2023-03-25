#ifndef DATAGRAPHTOOL_NETWORKING_BACKEND_UNITTESTS_H__
#define DATAGRAPHTOOL_NETWORKING_BACKEND_UNITTESTS_H__

#include <atomic>
#include <thread>

#include <Networking/backend/SocketWrapper.hpp>

#include <gtest/gtest.h>

namespace DataGraph::tests
{
class ConnectionServer: public ::testing::Environment
{
public:
	ConnectionServer(std::string port);
	void SetUp() override;
	void TearDown() override;

	static const std::string_view getIp();
	static const std::string_view getPort();

	static std::atomic<bool> m_emptyResponse;
	static std::atomic<const char*> m_customResponse;

private:
	std::thread m_thread;

	Networking::backend::SocketWrapper m_listeningSocket;

	std::atomic_bool m_threadRunning;

	static std::string m_serverIp;
	static std::string m_serverPort;
};
}  // namespace DataGraph::tests
#endif
