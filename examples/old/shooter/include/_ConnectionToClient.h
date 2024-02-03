#ifndef CONNECTIONTOCLIENTE_H
#define CONNECTIONTOCLIENTE_H
#include "Connection.h"

class ConnectionToClient : public Connection
{
public:
	ConnectionToClient(TCPsocket socket);
};

#endif // CONNECTIONTOCLIENTE_H
