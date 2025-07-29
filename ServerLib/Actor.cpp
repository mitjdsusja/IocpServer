#include "pch.h"
#include "Job.h"
#include "Actor.h"
#include "JobScheduler.h"

void Actor::PushJob(unique_ptr<Job> job){

	_jobQueue.Push(move(job));

	bool expected = false;
	if (_pendingJob.compare_exchange_strong(expected, true)) {
		GJobScheduler->PushActor(shared_from_this());
	}
}

void Actor::ExecuteJob(){

	vector<unique_ptr<Job>> jobsToExecute;
	_jobQueue.PopAll(jobsToExecute);

	for (unique_ptr<Job>& job : jobsToExecute) {
		job->Execute();

		int64 latency = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - job->GetCreateTimePoint()).count();
		RecordJobRatency(latency);
	}

	// Execute�� �۾��� queue�� �߰� ������ �� ����.
	bool scheduleNeeded = false;
	{
		// Execute�� �۾��� �߰��� ���
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

int64 Actor::GetAvgJobLatency(){

	if (_latencyQueue.empty()) return 0;
	return _latencySum / _latencyQueue.size();
}

