/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __CONNECTION_MANAGER_H__
#define __CONNECTION_MANAGER_H__

#include "NetworkTables/Data.h"
#include "Task.h"
#include <set>

namespace NetworkTables
{
class Connection;

class ConnectionManager
{
	friend class Connection;
public:
	static void Initialize();
	static void Shutdown();
	static const int kPort;
private:
	static int AcceptConnections();
	static void AddConnection(Connection *connection);
	static void RemoveConnection(Connection *connection);

	typedef std::set<Connection *> ConnectionSet_t;
	static bool _isServer;
	static bool _initialized;
	static ConnectionSet_t _connections;
	static Task *_listener;
	static bool _run;
};

} // namespace

#endif // __CONNECTION_MANAGER_H__
