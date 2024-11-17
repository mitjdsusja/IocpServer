#pragma once
#include <functional>
#include "ServerPch.h"

#include "LockQueue.h"

using namespace std;

class Job {
public:
	Job(function<void()> callback) : _callback(callback) {

	}

	template<typename T, typename Ret, typename... Args>
	Job(T* owner, Ret(T::* memFunc)(Args...), Args... args) {
		_callback = [owner, memFunc, args...]() {
			(owner->*memFunc)(args...);
			};
	}

	void Excute() {
		_callback();
	}

private:
	function<void()> _callback;

};

class JobQueue {
public:
	void DoAsync(function<void()> callback) {
		Push(new Job(callback));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsynd(Ret(T::* memFunc)(Args...), Args... args) {
		T* owner = (T*)this;
		Push(new Job(owner, memFunc, args...));
	}

public:
	void Push(Job* job);
	void Execute();

private:
	LockQueue<Job*> _jobs;
	atomic<int32> _jobCount = 0;
};