#include <posta/Network/TCPConnOfServer.h>
// Include htonl and others
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

using posta::TCPConnOfServer;

TCPConnOfServer::TCPConnOfServer(uint16_t port)
{
	//std::cout << "hello?" << std::endl;
	IPaddress address;
	address.host = INADDR_ANY;
	address.port = htons(port);
	set_socket(SDLNet_TCP_Open(&address));
	//std::cout << "hella?" << std::endl;
	if (get_socket() == nullptr)
		throw std::logic_error("Couldn't open server socket");
}

posta::TCPConnToClient* TCPConnOfServer::accept_incoming_connection()
{
	TCPsocket sock = SDLNet_TCP_Accept(get_socket());
	if (!sock)
		return nullptr;
	return new posta::TCPConnToClient(sock);
}

