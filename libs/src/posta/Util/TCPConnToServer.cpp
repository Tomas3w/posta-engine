#include <posta/Util/TCPConnToServer.h>
#include <posta/Util/LoggingMacro.h>

using Engine::TCPConnToServer;

std::unordered_map<IPaddress, std::pair<Engine::Thread, std::unique_ptr<TCPConnToServer>>> TCPConnToServer::connections;

bool operator==(const IPaddress &a, const IPaddress &b)
{
	return a.host == b.host && a.port == b.port;
}

static void connect_to_server(IPaddress ip)
{
	//LOG("thread running!");
	TCPsocket sock = SDLNet_TCP_Open(&ip);
	TCPConnToServer* r = nullptr;
	if (sock)
		r = new TCPConnToServer(sock);
	TCPConnToServer::connections[ip].second.reset(r);
	//LOG("thread ran!");
}

TCPConnToServer* TCPConnToServer::connect_to(IPaddress address)
{
	//std::pair<Uint32, Uin16> ip(address.host, address.port);
	//LOG("noxd");
	if (connections[address].first)
	{
		if (!connections[address].first.is_running())
		{
			//LOG("not running...");
			connections[address].first.reset();
			return connections[address].second.release();
		}
		//else
		//	LOG("something running... ", connections.size());
	}
	else
		connections[address].first.reset(connect_to_server, address);
	//LOG("xd");
	return nullptr;
}

TCPConnToServer::TCPConnToServer(TCPsocket socket)
{
	set_socket(socket);
}

