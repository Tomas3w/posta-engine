#include <posta/Util/TCPConnToClient.h>

using posta::TCPConnToClient;

TCPConnToClient::TCPConnToClient(TCPsocket socket)
{
	set_socket(socket);
}

