#pragma once
#include <thread>
#include <functional>

class ThreadManager{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(function<void(void)> callback);
	void Join();

private:
	mutex _threadsMutex;
	vector<thread> _threads;

};

