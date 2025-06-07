#pragma once
#include "LockQueue.h"

class Job;
class Job;

class JobQueueBase : public enable_shared_from_this<JobQueueBase>{
public:
	virtual ~JobQueueBase() = default;

	void PushJob(unique_ptr<Job> job);
	void ExecuteJob();

protected:
	atomic<bool> _pendingJob = false;
	LockQueue<unique_ptr<Job>> _jobQueue;
};
