#pragma once
#include "LockQueue.h"
#include "Job.h"

class Actor : public enable_shared_from_this<Actor>{
public:
	virtual ~Actor() = default;

	void PushJob(unique_ptr<Job> job);
	void ExecuteJob();

	uint64 GetActorId() { return _actorId; }
	int64 GetAvgJobLatency();

	void SetActorId(uint64 actorId);

private:
	void RecordJobRatency(int64 us);

protected:
	uint64 _actorId = 0;

	atomic<bool> _pendingJob = false;
	LockQueue<unique_ptr<Job>> _jobQueue;

private:
	queue<int64> _latencyQueue;
	const int16 _maxLatencyQueueSize = 100;
	int64 _latencySum = 0;
};
