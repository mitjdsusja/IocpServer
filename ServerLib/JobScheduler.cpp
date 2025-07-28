#include "pch.h"
#include "JobScheduler.h"
#include "JobTimer.h"
#include "Actor.h"

void JobScheduler::PushActor(shared_ptr<Actor> actor){

	_scheduledActor.Push(actor);

	_cv.notify_one();
}

shared_ptr<Actor> JobScheduler::PopActor(){

	unique_lock<mutex> lock(_actorMutex);
	_cv.wait(lock, [this]() {
		return _scheduledActor.Empty() == false;
	});

	return _scheduledActor.Pop();
}

void JobScheduler::RegisterTimedJob(shared_ptr<ScheduledTimedJob> scheduledTimedJob){

	lock_guard<mutex> lock(_timedActor);

	_scheduledTimedActor.push(scheduledTimedJob);
}

void JobScheduler::CheckTimedJob(){

	if (_isEnqueuing.exchange(true) == true) {
		return;
	}

	vector<unique_ptr<TimedJob>> timedJobs;
	vector<shared_ptr<Actor>> jobQueues;
	{
		lock_guard<mutex> lock(_timedActor);

		while (_scheduledTimedActor.empty() == false) {
			shared_ptr<ScheduledTimedJob> _scheduledTimedJob = _scheduledTimedActor.top();
			if (GetTickCount64() < _scheduledTimedJob->_timedJobRef->_executeTick) {
				break;
			}

			timedJobs.push_back(move(_scheduledTimedJob->_timedJobRef));
			jobQueues.push_back(_scheduledTimedJob->_jobQueueRef);
			_scheduledTimedActor.pop();
		}
	}

	//cout << "[JobScheduler::CheckTimedJob] TimedJob : " << timedJobs.size() << endl;
	for (int16 index = 0; index < timedJobs.size(); index++) {

		jobQueues[index]->PushJob(move(timedJobs[index]));
	}

	_isEnqueuing.store(false);
}
