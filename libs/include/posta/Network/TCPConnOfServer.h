#ifndef POSTAENGINE_TCPCONNOFSERVER_H
#define POSTAENGINE_TCPCONNOFSERVER_H
#include <posta/Network/TCPConnToClient.h>

namespace posta {
	class TCPConnOfServer : public TCPConn
	{
	public:
		TCPConnOfServer(uint16_t port);

		TCPConnToClient* accept_incoming_connection();
	};
}
#endif // POSTAENGINE_TCPCONNOFSERVER_H
