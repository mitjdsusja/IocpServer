#pragma once
#include "LockQueue.h"

class JobQueueBase;

class JobScheduler{
public:
	void PushJobQueue(shared_ptr<JobQueueBase> JobQueue);
	shared_ptr<JobQueueBase> PopJobQueue();

private:
	LockQueue<shared_ptr<JobQueueBase>> _scheduledJobQueue;
};

