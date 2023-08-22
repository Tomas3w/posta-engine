#ifndef POSTAENGINE_TCPCONNTOCLIENTE_H
#define POSTAENGINE_TCPCONNTOCLIENTE_H
#include <posta/Network/TCPConn.h>

namespace posta {
	class TCPConnToClient : public TCPConn
	{
	public:
		TCPConnToClient(TCPsocket socket);
	};
}

#endif // POSTAENGINE_TCPCONNTOCLIENTE_H
