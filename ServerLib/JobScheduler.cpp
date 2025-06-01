#include "pch.h"
#include "JobScheduler.h"
#include "JobQueue.h"

void JobScheduler::PushJobQueue(shared_ptr<IJobQueue> jobQueue){

	lock_guard<mutex> lock(_jobQueueMutex);

	_scheduledJobQueue.push(jobQueue);
}

shared_ptr<IJobQueue> JobScheduler::PopJobQueue(){

	lock_guard<mutex> lock(_jobQueueMutex);

	shared_ptr<IJobQueue> jobQueue = _scheduledJobQueue.front();
	_scheduledJobQueue.pop();

	return jobQueue;
}
