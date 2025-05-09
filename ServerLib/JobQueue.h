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

		uint64 popTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
		uint16 elapsedTime = popTime - job->GetCreateTime();
		_enqueueToDequeueTimeQueue.push_back(elapsedTime);
		_enqueueToDequeueTimeSum += elapsedTime;
		//cout << "createTime : " << job->GetCreateTime() << endl;
		//cout << "poptime : " << popTime << endl;
		//cout << "Elapsed Time : " << elapsedTime << endl;
		//cout << elapsedTime << endl;
		if (_enqueueToDequeueTimeQueue.size() > 100) {
			uint16 time = _enqueueToDequeueTimeQueue.front();
			_enqueueToDequeueTimeQueue.pop_front();
			_enqueueToDequeueTimeSum -= time;
		}

		return job;
	}

	uint64 GetJobCreateTimeAvg() {

		return _enqueueToDequeueTimeSum / _enqueueToDequeueTimeQueue.size();
	}

private:
	mutex _queueMutex;
	condition_variable _cv;

	queue<Job*> _jobQueue;
	deque<int16> _enqueueToDequeueTimeQueue;
	uint64 _enqueueToDequeueTimeSum = 0;
};