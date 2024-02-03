#include "ConnectionToServer.h"

ConnectionToServer* ConnectionToServer::connect_to(IPaddress address)
{
	TCPsocket sock = SDLNet_TCP_Open(&address);
	if (sock)
		return new ConnectionToServer(sock);
	return nullptr;
}

ConnectionToServer::ConnectionToServer(TCPsocket socket)
{
	set_socket(socket);
}

