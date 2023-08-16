#ifndef POSTAENGINE_TCPCONNTOSERVER_H
#define POSTAENGINE_TCPCONNTOSERVER_H
#include <posta/Util/TCPConn.h>
#include <posta/Util/Thread.h>
#include <thread>
#include <unordered_map>

namespace std {
	template <>
	struct hash<IPaddress>
	{
		std::size_t operator()(const IPaddress& k) const
		{
			return hash<Uint32>()(k.host) ^ (hash<Uint16>()(k.port) << 1);
		}
	};
}

bool operator==(const IPaddress &a, const IPaddress &b);

namespace posta {
	class TCPConnToServer : public TCPConn
	{
	public:
		static std::unordered_map<IPaddress, std::pair<Thread, std::unique_ptr<TCPConnToServer>>> connections;
		static TCPConnToServer* connect_to(IPaddress address);
		TCPConnToServer(TCPsocket socket);
	};
}

#endif // POSTAENGINE_TCPCONNTOSERVER_H
