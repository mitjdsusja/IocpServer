#include "pch.h"
#include "JobScheduler.h"
#include "JobTimer.h"
#include "JobQueue.h"

void JobScheduler::PushJobQueue(shared_ptr<JobQueueBase> jobQueue){

	_scheduledJobQueue.Push(jobQueue);

	_cv.notify_one();
}

shared_ptr<JobQueueBase> JobScheduler::PopJobQueue(){

	unique_lock<mutex> lock(_jobQueueMutex);
	_cv.wait(lock, [this]() {
		return _scheduledJobQueue.Empty() == false;
	});

	return _scheduledJobQueue.Pop();
}

void JobScheduler::RegisterTimedJob(shared_ptr<ScheduledTimedJob> scheduledTimedJob){

	lock_guard<mutex> lock(_timedJobQueue);

	scheduledTimedJob->_timedJobRef->_executeTick = GetTickCount64();
	_scheduledTimedJobQueue.push(scheduledTimedJob);

	_isEnqueuing.store(true);
}

void JobScheduler::CheckTimedJob(){

	if (_isEnqueuing.exchange(true) == true) {
		return;
	}

	vector<unique_ptr<TimedJob>> timedJobs;
	vector<shared_ptr<JobQueueBase>> jobQueues;
	{
		lock_guard<mutex> lock(_timedJobQueue);

		while (_scheduledTimedJobQueue.empty() == false) {
			shared_ptr<ScheduledTimedJob> _scheduledTimedJob = _scheduledTimedJobQueue.top();
			if (GetTickCount64() < _scheduledTimedJob->_timedJobRef->_executeTick) {
				break;
			}

			timedJobs.push_back(move(_scheduledTimedJob->_timedJobRef));
			jobQueues.push_back(_scheduledTimedJob->_jobQueueRef);
			_scheduledTimedJobQueue.pop();
		}
	}

	for (int16 index = 0; index < timedJobs.size(); index++) {

		jobQueues[index]->PushJob(move(timedJobs[index]));
	}

	_isEnqueuing.store(false);
}
