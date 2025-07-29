#include "pch.h"
#include "ActorManager.h"

#include "Job.h"
#include "Actor.h"
#include "MonitorManager.h"

wstring ActorManager::TypeToWstring(ActorType actorType){

	switch (actorType) {
	case ActorType::RoomManagerType: return L"RoomManager";
	case ActorType::PlayerManagerType: return L"PlayerManager";
	case ActorType::RoomType: return L"Room";
	case ActorType::PlayerType: return L"Player";
	default: return L"None";
	}
}

uint64 ActorManager::RegisterActor(shared_ptr<Actor> actor){

	uint64 actorId = GenerateId();

	lock_guard<mutex> lock(_actorsMutex);
	_actors.insert({ actorId, actor });

	return actorId;
}

void ActorManager::UnRegisterActor(uint64 actorId){

	lock_guard<mutex> lock(_actorsMutex);

	_actors.erase(actorId);
}

void ActorManager::RequestAllLatencyAndSendToMonitor(){

	shared_ptr<mutex> vectorMutexRef = make_shared<mutex>();
	shared_ptr<vector<pair<uint64, uint64>>> latencyVectorRef = make_shared< vector<pair<uint64, uint64>>>();
	shared_ptr<wstring> msgRef = make_shared<wstring>();
	int64 actorCount = 0;
	{
		lock_guard<mutex> lock(_actorsMutex);
		actorCount = _actors.size();
	}

	for (auto& p : _actors) {
		
		shared_ptr<Actor>& actor = p.second;

		unique_ptr<Job> job = make_unique<Job>([actor, vectorMutexRef, msgRef, latencyVectorRef, actorCount]() {

			uint64 actorId = actor->GetActorId();
			uint64 latency = actor->GetAvgJobLatency();
			ActorType actorType = actor->GetActorType();

			lock_guard<mutex> _lock(*vectorMutexRef);
			latencyVectorRef->push_back({ actorId, latency });
			
			*msgRef += ActorManager::TypeToWstring(actorType);
			*msgRef += to_wstring(actorId);
			*msgRef += L" : ";
			*msgRef += to_wstring(latency);
			*msgRef += L"us \n";

			if (actorCount == latencyVectorRef->size()) {

				GMonitorManager->PushJobSendMsg(*msgRef);
			}
		});

		actor->PushJob(move(job));
	}
}

uint64 ActorManager::GetActorCount(){

	lock_guard<mutex> _lock(_actorsMutex);

	return _actors.size();
}

uint64 ActorManager::GenerateId(){

	static atomic<uint64> actorId = 1;
	return actorId.fetch_add(1);
}
