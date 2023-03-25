#ifndef DATAGRAPHTOOL_NETWORKING_BACKEND_SOCKETWRAPPER_H__
#define DATAGRAPHTOOL_NETWORKING_BACKEND_SOCKETWRAPPER_H__

#include <utility>
#include <WinSock2.h>

namespace DataGraph::Networking::backend
{
class SocketWrapper
{
public:
	SocketWrapper();
	SocketWrapper(SOCKET sock);

	SocketWrapper(SocketWrapper&& other) noexcept { *this = std::move(other); }
	SocketWrapper& operator=(SocketWrapper&& other) noexcept
	{
		m_socket = other.m_socket;
		other.m_socket = INVALID_SOCKET;
		return *this;
	}

	SOCKET operator&();
	SOCKET get();

	~SocketWrapper();

private:
	SOCKET m_socket;
};
}  // namespace DataGraph::Networking::backend
#endif	// !1
