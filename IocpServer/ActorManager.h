#pragma once

class Actor;

class ActorManager{
public:
	uint64 RegisterActor(shared_ptr<Actor> actor);
	void UnRegisterActor(uint64 actorId);

	void RequestAllLatency();

	uint64 GetActorCount();

private:
	uint64 GenerateId();

private:
	mutex _actorsMutex;
	unordered_map<uint64, shared_ptr<Actor>> _actors;

};

