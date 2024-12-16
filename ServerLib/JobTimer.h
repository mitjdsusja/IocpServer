#pragma once
#include <functional>

#include "Job.h"

class TimedJob : public Job{
public:
	TimedJob(uint32 executeTick, function<void()> callback) : Job(callback), _executeTick(executeTick){

	}
	
public:
	uint64 _executeTick = 0;

};

class JobTimer{
public:
	void Reserve(uint64 delayTime, function<void()>&& callback);
	void EnqueueReadyJobs(JobQueue& jobQueue);
	
private:
	mutex _queueMutex;
	priority_queue<TimedJob*> _timedJobQueue;
	atomic<bool> _isEnqueuing;
};

