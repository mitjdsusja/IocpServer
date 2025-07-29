#pragma once
#include <functional>

#include "Job.h"

class TimedJob : public Job {
public:
	TimedJob(uint64 executeTick, function<void()>&& callback) : Job(move(callback)), _executeTick(executeTick + GetTickCount64()) {

	}
	~TimedJob() {
		//cout << "TimedJob destructor" << endl;
	}

public:
	uint64 _executeTick = 0;

};

struct TimedJobComparer {
	bool operator()(TimedJob* lhs, TimedJob* rhs) const {
		return lhs->_executeTick > rhs->_executeTick;
	}
};

class JobTimer {
public:
	void Reserve(uint64 delayTime, function<void()>&& callback);
	//void EnqueueReadyJobs(JobQueue& jobQueue);

private:
	mutex _queueMutex;
	priority_queue<TimedJob*, vector<TimedJob*>, TimedJobComparer> _timedActor;
	atomic<bool> _isEnqueuing;
};

