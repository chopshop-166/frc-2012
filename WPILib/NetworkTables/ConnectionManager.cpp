/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "NetworkTables/ConnectionManager.h"

#include "NetworkTables/Connection.h"
#include "NetworkTables/InterfaceConstants.h"
#include <inetLib.h>
#include <sockLib.h>
#include <taskLib.h>

namespace NetworkTables
{

const int ConnectionManager::kPort = 1735;
bool ConnectionManager::_isServer = true;
bool ConnectionManager::_initialized = false;
ConnectionManager::ConnectionSet_t ConnectionManager::_connections;
Task *ConnectionManager::_listener = NULL;
bool ConnectionManager::_run = true;

void ConnectionManager::Initialize()
{
	if (!_initialized)
	{
		_listener = new Task("NetworkTablesListener", (FUNCPTR)AcceptConnections);
		_initialized = true;
		_listener->Start();
	}
}

void ConnectionManager::Shutdown()
{
	_run = false;
	while(_listener->Verify())
		taskDelay(10);
	delete _listener;
}

int ConnectionManager::AcceptConnections()
{
	int listenSocket;
	struct sockaddr_in serverAddr;
	int sockAddrSize = sizeof(serverAddr);
	bzero((char *)&serverAddr, sockAddrSize);
	serverAddr.sin_len = (u_char)sockAddrSize;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(kPort);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Create the socket.
	if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{
		return -1;
	}

	// Set the TCP socket so that it can be reused if it is in the wait state.
	int reuseAddr = 1;
	setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseAddr, sizeof(reuseAddr));

	// Bind socket to local address.
	if (bind(listenSocket, (struct sockaddr *)&serverAddr, sockAddrSize) == ERROR)
	{
		close(listenSocket);
		return -1;
	}

	while (_run)
	{
		// Create queue for client connection requests.
		if (listen(listenSocket, 1) == ERROR)
			continue;

		struct sockaddr clientAddr;
		int clientAddrSize;
		int connectedSocket = accept(listenSocket, &clientAddr, &clientAddrSize);
		if (connectedSocket == ERROR)
			continue;

		// TODO: Linger option?
		AddConnection(new Connection(connectedSocket));
	}
	return 0;
}

void ConnectionManager::AddConnection(Connection *connection)
{
	_connections.insert(connection);
	connection->Start();
}

void ConnectionManager::RemoveConnection(Connection *connection)
{
	_connections.erase(connection);
}

} // namespace
