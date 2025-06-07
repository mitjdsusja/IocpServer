#include "pch.h"
#include "JobScheduler.h"

void JobScheduler::PushJobQueue(shared_ptr<JobQueueBase> jobQueue){

	{
		lock_guard<mutex> lock(_mutex);
		_scheduledJobQueue.Push(jobQueue);
	}

	_cv.notify_one();
}

shared_ptr<JobQueueBase> JobScheduler::PopJobQueue(){

	unique_lock<mutex> lock(_mutex);
	_cv.wait(lock, [this]() {
		return _scheduledJobQueue.Empty() == false;
	});

	return _scheduledJobQueue.Pop();
}
