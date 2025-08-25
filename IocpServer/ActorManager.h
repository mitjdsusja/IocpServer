#pragma once

class Actor;

enum ActorType {

	NONE = 0,

	ROOM_MANAGER_TYPE = 1,
	PLAYER_MANAGER_TYPE = 2,
	MONITOR_MANAGER_TYPE = 3,

	ROOM_TYPE = 101,
	PLAYER_TYPE = 102,
};

struct ActorInfo {

	uint64 actorId = 0;
	uint64 latency = 0;
	ActorType actorType = NONE;
};

class ActorManager{
public:
	static wstring TypeToWstring(ActorType actorType);

	uint64 RegisterActor(shared_ptr<Actor> actor);
	void UnRegisterActor(uint64 actorId);

	void RequestAllLatencyAndSendToMonitor();

	uint64 GetActorCount();

private:
	uint64 GenerateId();

private:
	mutex _actorsMutex;
	unordered_map<uint64, shared_ptr<Actor>> _actors;

};

