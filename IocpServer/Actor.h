#pragma once
#include "LockQueue.h"
#include "Job.h"

enum ActorType;

class Actor : public enable_shared_from_this<Actor> {
public:
	Actor(ActorType type);
	virtual ~Actor() = default;

	void PushJob(unique_ptr<Job> job);
	void ExecuteJob();

	ActorType GetActorType();
	uint64 GetActorId() { return _actorId; }
	int64 GetAvgJobLatency();

	void SetActorId(uint64 actorId);

private:
	void RecordJobRatency(int64 us);

protected:
	uint64 _actorId = 0;
	ActorType _actorType;

	atomic<bool> _pendingJob = false;
	LockQueue<unique_ptr<Job>> _jobQueue;

private:
	queue<int64> _latencyQueue;
	const int16 _maxLatencyQueueSize = 10;
	int64 _latencySum = 0;
};
