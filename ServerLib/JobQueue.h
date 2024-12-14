#pragma once
#include <functional>
#include "ServerPch.h"

class JobQueue {
public:
	void Push(function<void()> func) {
		{
			lock_guard<mutex> lock(_queueMutex);
			_jobQueue.push(func);
		}
		_cv.notify_one();
	}
	function<void()> Pop() {
		unique_lock<mutex> lock(_queueMutex);

		while (_jobQueue.empty()) {
			_cv.wait(lock);
		}
		
		function<void()> func = _jobQueue.front();
		_jobQueue.pop();
		return func;
	}

private:
	mutex _queueMutex;
	condition_variable _cv;

	queue<function<void()>> _jobQueue;
};