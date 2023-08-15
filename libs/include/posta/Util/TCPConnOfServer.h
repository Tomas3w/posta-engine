#ifndef POSTAENGINE_TCPCONNOFSERVER_H
#define POSTAENGINE_TCPCONNOFSERVER_H
#include <engine/include/Util/TCPConnToClient.h>

namespace Engine {
	class TCPConnOfServer : public TCPConn
	{
	public:
		TCPConnOfServer(short port);

		TCPConnToClient* accept_incoming_connection();
	};
}
#endif // POSTAENGINE_TCPCONNOFSERVER_H
