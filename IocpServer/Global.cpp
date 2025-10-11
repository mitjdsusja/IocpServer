#include "pch.h"
#include "Global.h"

#include "ActorManager.h"
#include "RoomManager.h"
#include "PlayerManager.h"

#include "MapManager.h"

#include "JobScheduler.h"
#include "MonitorManager.h"

shared_ptr<RoomManager> GRoomManager = nullptr;
shared_ptr<PlayerManager> GPlayerManager = nullptr;
shared_ptr<ActorManager> GActorManager = nullptr;

shared_ptr<MapManager> GMapManager = nullptr;

shared_ptr<JobScheduler> GJobScheduler = nullptr;
shared_ptr<MonitorManager> GMonitorManager = nullptr;

chrono::steady_clock::time_point GServerStartTimePoint;

class Global {
public :
	Global() {
		GActorManager = make_shared<ActorManager>();
		GRoomManager = make_shared<RoomManager>();
		GPlayerManager = make_shared<PlayerManager>();

		GMapManager = make_shared<MapManager>();	

		GRoomManager->SetActorId(GActorManager->RegisterActor(GRoomManager));
		GPlayerManager->SetActorId(GActorManager->RegisterActor(GPlayerManager));

		GJobScheduler = make_shared<JobScheduler>();
		GMonitorManager = make_shared<MonitorManager>();

		GServerStartTimePoint = chrono::steady_clock::now();
	}
	~Global() {

	}
}Global;