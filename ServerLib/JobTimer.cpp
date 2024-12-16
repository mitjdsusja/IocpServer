#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"

void JobTimer::Reserve(uint64 delayTime, function<void()>&& callback){
	uint64 executeTick = GetTickCount64() + delayTime;
	TimedJob* timedJob = new TimedJob(executeTick, move(callback));

	lock_guard<mutex> lock(_queueMutex);
	_timedJobQueue.push(timedJob);
}

void JobTimer::EnqueueReadyJobs(JobQueue& jobQueue){
	if (_isEnqueuing.exchange(true) == true) {
		return;
	}

	vector<TimedJob*> jobs;
	{
		lock_guard<mutex> lock(_queueMutex);

		while (_timedJobQueue.empty() == false) {
			TimedJob* timedJob = _timedJobQueue.top();
			if (GetTickCount64() < timedJob->_executeTick) {
				break;
			}

			jobs.push_back(timedJob);
			_timedJobQueue.pop();
		}
	}

	for (TimedJob* job : jobs) {
		jobQueue.Push(job);
	}

	_isEnqueuing.store(false);
}
