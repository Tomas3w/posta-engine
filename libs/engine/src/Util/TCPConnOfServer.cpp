#include <engine/include/Util/TCPConnOfServer.h>

using Engine::TCPConnOfServer;

TCPConnOfServer::TCPConnOfServer(short port)
{
	//std::cout << "hello?" << std::endl;
	IPaddress address;
	address.host = INADDR_ANY;
	address.port = port;
	set_socket(SDLNet_TCP_Open(&address));
	//std::cout << "hella?" << std::endl;
	if (get_socket() == nullptr)
		throw std::logic_error("Couldn't open server socket");
}

Engine::TCPConnToClient* TCPConnOfServer::accept_incoming_connection()
{
	TCPsocket sock = SDLNet_TCP_Accept(get_socket());
	if (!sock)
		return nullptr;
	return new Engine::TCPConnToClient(sock);
}

