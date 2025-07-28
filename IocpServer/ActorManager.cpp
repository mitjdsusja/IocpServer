#include "pch.h"
#include "ActorManager.h"

uint64 ActorManager::RegisterActor(shared_ptr<Actor> actor){

	uint64 actorId = GenerateId();

	lock_guard<mutex> _lock(_actorsMutex);
	_actors.insert({ actorId, actor });

	return actorId;
}

void ActorManager::UnRegisterActor(uint64 actorId){


}

void ActorManager::RequestAllLatency(){


}

uint64 ActorManager::GetActorCount(){

	lock_guard<mutex> _lock(_actorsMutex);

	return _actors.size();
}

uint64 ActorManager::GenerateId(){

	static atomic<uint64> actorId = 1;
	return actorId.fetch_add(1);
}
