#include "pch.h"
#include "Job.h"
#include "JobQueue.h"
#include "JobScheduler.h"

void Actor::PushJob(unique_ptr<Job> job){

	_jobQueue.Push(move(job));

	bool expected = false;
	if (_pendingJob.compare_exchange_strong(expected, true)) {
		GJobScheduler->PushJobQueue(shared_from_this());
	}
}

void Actor::ExecuteJob(){

	vector<unique_ptr<Job>> jobsToExecute;
	_jobQueue.PopAll(jobsToExecute);

	for (unique_ptr<Job>& job : jobsToExecute) {
		job->Execute();
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
		GJobScheduler->PushJobQueue(shared_from_this());
	}
}

void Actor::RecordJobRatency(int64 us) {

	_totalLatency += us;
	_processedJobCount++;
}

void Actor::SetActorId(uint64 actorId) {

	_actorId = actorId;
}

int64 Actor::GetAvgJobRatency(){

	return _totalLatency / _processedJobCount;
}

