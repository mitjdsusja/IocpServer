#include "pch.h"
#include "Global.h"

#include "RoomManager.h"
#include "PlayerManager.h"
#include "MonitorManager.h"

shared_ptr<RoomManager> GRoomManager = nullptr;
shared_ptr<PlayerManager> GPlayerManager = nullptr;
shared_ptr<MonitorManager> GMonitorManager = nullptr;

class Global {
public :
	Global() {
		GRoomManager = make_shared<RoomManager>();
		GPlayerManager = make_shared<PlayerManager>();
		GMonitorManager = make_shared<MonitorManager>();
	}
	~Global() {
		//delete GRoomManager;
		//delete GPlayerManager;
	}
}Global;