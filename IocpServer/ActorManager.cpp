#include "pch.h"
#include "ActorManager.h"

#include "Job.h"
#include "Actor.h"
#include "MonitorManager.h"

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
	shared_ptr<vector<pair<uint64, uint64>>> latencyVector = make_shared< vector<pair<uint64, uint64>>>();
	int64 actorCount = 0;
	{
		lock_guard<mutex> lock(_actorsMutex);
		actorCount = _actors.size();
	}

	for (auto& p : _actors) {
		
		shared_ptr<Actor>& actor = p.second;

		unique_ptr<Job> job = make_unique<Job>([actor, vectorMutexRef, latencyVectorRef = latencyVector, actorCount]() {

			uint64 actorId = actor->GetActorId();
			uint64 latency = actor->GetAvgJobLatency();

			lock_guard<mutex> _lock(*vectorMutexRef);
			latencyVectorRef->push_back({ actorId, latency });

			if (actorCount == latencyVectorRef->size()) {

				wstring msg;
				for (auto& p : *latencyVectorRef) {

					msg += L"Actor";
					msg += to_wstring(p.first);
					msg += L" : ";
					msg += to_wstring(p.second);
					msg += L"us";
					msg += L"\n";
				}
				GMonitorManager->PushJobSendMsg(msg);
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
