#pragma once
#include "LockQueue.h"

class Job;

class JobQueueBase : public enable_shared_from_this<JobQueueBase>{
public:
	virtual ~JobQueueBase() = default;

	void PushJob(unique_ptr<Job> job);
	void ExecuteJob();

private:
	void RecordJobRatency(int64 us);
	int64 GetAvgJobRatency();

protected:
	atomic<bool> _pendingJob = false;
	LockQueue<unique_ptr<Job>> _jobQueue;

private:
	int64 _totalLatency = 0;
	int64 _processedJobCount = 0;
};
