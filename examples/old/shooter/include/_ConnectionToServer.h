#ifndef CONNECTIONTOSERVER_H
#define CONNECTIONTOSERVER_H
#include "Connection.h"

class ConnectionToServer : public Connection
{
public:
	static ConnectionToServer* connect_to(IPaddress address);
	ConnectionToServer(TCPsocket socket);
};

#endif // CONNECTIONTOSERVER_H
