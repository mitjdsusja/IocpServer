#include "pch.h"
#include "Job.h"
#include "Actor.h"
#include "ActorManager.h"
#include "JobScheduler.h"

Actor::Actor(ActorType type = ActorType::NONE) : _actorType(type){

}

void Actor::PushJob(unique_ptr<Job> job) {

	job->RecordEnqueueTimePoint();
	_jobQueue.Push(move(job));

	bool expected = false;
	if (_pendingJob.compare_exchange_strong(expected, true)) {
		GJobScheduler->PushActor(shared_from_this());
	}
}

void Actor::ExecuteJob() {

	vector<unique_ptr<Job>> jobsToExecute;
	_jobQueue.PopAll(jobsToExecute);

	for (unique_ptr<Job>& job : jobsToExecute) {

		job->Execute();
		//spdlog::info("[Actor::ExecuteJob] Job Execute - execute Server Time : {}", chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - GServerStartTimePoint).count());

		int64 latency = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - job->GetEuqueueTimePoint()).count();
		RecordJobRatency(latency);
	}

	// Execute중 작업이 queue에 추가 되있을 수 있음.
	bool scheduleNeeded = false;
	{
		// Execute중 작업이 추가된 경우
		if (_jobQueue.Empty() == false) {
			scheduleNeeded = true;
		}
		else {
			_pendingJob.store(false);
		}
	}

	if (scheduleNeeded == true) {
		GJobScheduler->PushActor(shared_from_this());
	}
}

ActorType Actor::GetActorType(){

	return _actorType;
}

void Actor::RecordJobRatency(int64 us) {

	_latencyQueue.push(us);
	_latencySum += us;

	if (_latencyQueue.size() > _maxLatencyQueueSize) {

		_latencySum -= _latencyQueue.front();
		_latencyQueue.pop();
	}
}

void Actor::SetActorId(uint64 actorId) {

	_actorId = actorId;
}

int64 Actor::GetAvgJobLatency() {

	if (_latencyQueue.empty()) return 0;
	return _latencySum / _latencyQueue.size();
}

