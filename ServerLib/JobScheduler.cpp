#include "pch.h"
#include "JobScheduler.h"

void JobScheduler::PushJobQueue(shared_ptr<JobQueueBase> jobQueue){

	_scheduledJobQueue.Push(jobQueue);
}

shared_ptr<JobQueueBase> JobScheduler::PopJobQueue(){

	return _scheduledJobQueue.Pop();
}
