#ifndef CONNECTIONOFSERVER_H
#define CONNECTIONOFSERVER_H
#include "ConnectionToClient.h"

class ConnectionOfServer : public Connection
{
public:
	ConnectionOfServer(short port);

	ConnectionToClient* accept_incoming_connection();
};

#endif // CONNECTIONOFSERVER_H
