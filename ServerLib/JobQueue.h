#pragma once
#include "LockQueue.h"
#include "Job.h"

class JobQueueBase : public enable_shared_from_this<JobQueueBase>{
public:
	virtual ~JobQueueBase() = default;

	void PushJob(unique_ptr<Job> job);
	void ExecuteJob();

	int64 GetAvgJobRatency();

private:
	void RecordJobRatency(int64 us);

protected:
	atomic<bool> _pendingJob = false;
	LockQueue<unique_ptr<Job>> _jobQueue;

private:
	int64 _totalLatency = 0;
	int64 _processedJobCount = 0;
};
