#pragma once
#include "LockQueue.h"

class JobQueueBase;
class TimedJob;

struct ScheduledTimedJob {
public:
	unique_ptr<TimedJob> _timedJobRef;
	shared_ptr<JobQueueBase> _jobQueueRef;

};

class JobScheduler{
public:
	void PushJobQueue(shared_ptr<JobQueueBase> JobQueue);
	shared_ptr<JobQueueBase> PopJobQueue();

	void RegisterTimedJob(shared_ptr<ScheduledTimedJob> timedJob);
	void CheckTimedJob();

private:
	mutex _jobQueueMutex;
	LockQueue<shared_ptr<JobQueueBase>> _scheduledJobQueue;
	condition_variable _cv;

	mutex _timedJobQueue;
	atomic<bool> _isEnqueuing = false;
	priority_queue<shared_ptr<ScheduledTimedJob>> _scheduledTimedJobQueue;
};

