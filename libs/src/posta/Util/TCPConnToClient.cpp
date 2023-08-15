#include <posta/Util/TCPConnToClient.h>

using Engine::TCPConnToClient;

TCPConnToClient::TCPConnToClient(TCPsocket socket)
{
	set_socket(socket);
}

