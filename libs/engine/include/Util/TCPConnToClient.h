#ifndef POSTAENGINE_TCPCONNTOCLIENTE_H
#define POSTAENGINE_TCPCONNTOCLIENTE_H
#include <engine/include/Util/TCPConn.h>

namespace Engine {
	class TCPConnToClient : public TCPConn
	{
	public:
		TCPConnToClient(TCPsocket socket);
	};
}

#endif // POSTAENGINE_TCPCONNTOCLIENTE_H
