#pragma once
#include <functional>
#include "ServerPch.h"
#include "Job.h"

class JobQueue {
public:
	void Push(Job* job) {
		{
			lock_guard<mutex> lock(_queueMutex);
			_jobQueue.push(job);
		}
		_cv.notify_one();
	}
	Job* Pop() {
		unique_lock<mutex> lock(_queueMutex);

		while (_jobQueue.empty()) {
			_cv.wait(lock);
		}
		
		Job* job = _jobQueue.front();
		_jobQueue.pop();
		return job;
	}

private:
	mutex _queueMutex;
	condition_variable _cv;

	queue<Job*> _jobQueue;
};