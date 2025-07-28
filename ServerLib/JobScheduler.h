#pragma once
#include "LockQueue.h"
#include "JobTimer.h"

class Actor;
class TimedJob;

struct ScheduledTimedJob {
public:
	unique_ptr<TimedJob> _timedJobRef;
	shared_ptr<Actor> _jobQueueRef;

};

struct ScheduledTimedJobComparer {
	bool operator()(const shared_ptr<ScheduledTimedJob>& lhs, const shared_ptr<ScheduledTimedJob>& rhs) const {
		return lhs->_timedJobRef->_executeTick > rhs->_timedJobRef->_executeTick;
	}
};

class JobScheduler{
public:
	void PushActor(shared_ptr<Actor> actor);
	shared_ptr<Actor> PopActor();

	void RegisterTimedJob(shared_ptr<ScheduledTimedJob> timedJob);
	void CheckTimedJob();

private:
	mutex _actorMutex;
	LockQueue<shared_ptr<Actor>> _scheduledActor;
	condition_variable _cv;

	mutex _timedActor;
	atomic<bool> _isEnqueuing = false;
	priority_queue<
		shared_ptr<ScheduledTimedJob>,
		vector<shared_ptr<ScheduledTimedJob>>,
		ScheduledTimedJobComparer> _scheduledTimedActor;
};

