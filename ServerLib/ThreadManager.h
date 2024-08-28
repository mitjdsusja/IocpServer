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
	vector<thread> _threads;

};

