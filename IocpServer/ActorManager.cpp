#include "pch.h"
#include "ActorManager.h"
#include "Global.h"

#include "Job.h"
#include "Actor.h"
#include "MonitorManager.h"

wstring ActorManager::TypeToWstring(ActorType actorType){

	switch (actorType) {
	case ActorType::ROOM_MANAGER_TYPE: return L"RoomManager";
	case ActorType::PLAYER_MANAGER_TYPE: return L"PlayerManager";
	case ActorType::ROOM_TYPE: return L"Room";
	case ActorType::PLAYER_TYPE: return L"Player";
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
	shared_ptr<vector<ActorInfo>> actorInfoVectorRef = make_shared<vector<ActorInfo>>();
	shared_ptr<wstring> msgRef = make_shared<wstring>();
	shared_ptr<atomic<int32>> actorCount = make_shared<atomic<int32>>();
	
	*msgRef += L"Server Time : ";
	*msgRef += to_wstring(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - GServerStartTimePoint).count());
	*msgRef += L"\n";
	
	// Get Actors
	vector<shared_ptr<Actor>> actorList;
	{
		lock_guard<mutex> lock(_actorsMutex);

		for (auto& p : _actors) {
			
			actorList.push_back(p.second);
		}
	}

	const uint32 expectedActorCount = actorList.size();

	for (auto& actor : actorList) {

		unique_ptr<Job> job = make_unique<Job>([actor, vectorMutexRef, msgRef, actorInfoVectorRef, actorCount, expectedActorCount]() {

			ActorInfo actorInfo = { actor->GetActorId(), actor->GetAvgJobLatency(), actor->GetActorType() };

			{
				lock_guard<mutex> _lock(*vectorMutexRef);

				actorInfoVectorRef->push_back(actorInfo);
			}
			
			if (actorCount->fetch_add(1) + 1 == expectedActorCount) {

				int32 latencySum = 0;
				ActorInfo topLatencyActorInfo;
				for (auto& actorInfo : *actorInfoVectorRef) {

					latencySum += actorInfo.latency;
					if (topLatencyActorInfo.latency < actorInfo.latency) {
						topLatencyActorInfo = actorInfo;
					}
				}

				*msgRef += L"Total Actor Count : ";
				*msgRef += to_wstring(expectedActorCount);
				*msgRef += L"\n";

				*msgRef += L"latency avg : ";
				*msgRef += to_wstring(latencySum / expectedActorCount);
				*msgRef += L"us";
				*msgRef += L"\n";

				*msgRef += L"Top Latency Actor : ";
				*msgRef += ActorManager::TypeToWstring(topLatencyActorInfo.actorType);
				*msgRef += L" ";
				*msgRef += to_wstring(topLatencyActorInfo.latency);
				*msgRef += L"us";
				*msgRef += L"\n";

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
