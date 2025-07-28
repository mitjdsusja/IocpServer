#include "pch.h"
#include "Global.h"

#include "ActorManager.h"
#include "RoomManager.h"
#include "PlayerManager.h"
#include "MonitorManager.h"

shared_ptr<RoomManager> GRoomManager = nullptr;
shared_ptr<PlayerManager> GPlayerManager = nullptr;
shared_ptr<ActorManager> GActorManager = nullptr;

shared_ptr<MonitorManager> GMonitorManager = nullptr;

class Global {
public :
	Global() {
		GActorManager = make_shared<ActorManager>();
		GRoomManager = make_shared<RoomManager>();
		GPlayerManager = make_shared<PlayerManager>();

		GRoomManager->SetActorId(GActorManager->RegisterActor(GRoomManager));
		GPlayerManager->SetActorId(GActorManager->RegisterActor(GPlayerManager));

		GMonitorManager = make_shared<MonitorManager>();
	}
	~Global() {
		//delete GRoomManager;
		//delete GPlayerManager;
	}
}Global;