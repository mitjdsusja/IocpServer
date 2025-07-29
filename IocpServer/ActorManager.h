#pragma once

class Actor;

enum ActorType {

	None = 0,

	RoomManagerType = 1,
	PlayerManagerType = 2,
	MonitorManagerType = 3,

	RoomType = 101,
	PlayerType = 102,
};

class ActorManager{
public:
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

