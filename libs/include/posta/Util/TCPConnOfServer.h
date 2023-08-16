#ifndef POSTAENGINE_TCPCONNOFSERVER_H
#define POSTAENGINE_TCPCONNOFSERVER_H
#include <posta/Util/TCPConnToClient.h>

namespace posta {
	class TCPConnOfServer : public TCPConn
	{
	public:
		TCPConnOfServer(short port);

		TCPConnToClient* accept_incoming_connection();
	};
}
#endif // POSTAENGINE_TCPCONNOFSERVER_H
