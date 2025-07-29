#pragma once

class RoomManager;
class PlayerManager;
class ActorManager;

class JobScheduler;
class MonitorManager;

extern shared_ptr<RoomManager> GRoomManager;
extern shared_ptr<PlayerManager> GPlayerManager;
extern shared_ptr<ActorManager> GActorManager;

extern shared_ptr<JobScheduler> GJobScheduler;
extern shared_ptr<MonitorManager> GMonitorManager;
