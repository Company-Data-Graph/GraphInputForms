#include "SocketWrapper.hpp"

namespace DataGraph::Networking::backend
{
SocketWrapper::SocketWrapper()
 : m_socket(INVALID_SOCKET)
{}
SocketWrapper::SocketWrapper(SOCKET sock)
 : m_socket(sock)
{}
SOCKET SocketWrapper::get() { return m_socket; }
SOCKET SocketWrapper::operator&() { return m_socket; }
SocketWrapper::~SocketWrapper()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}
}  // namespace DataGraph::Networking::backend
