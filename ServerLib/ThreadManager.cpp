#include "pch.h"
#include "ThreadManager.h"

ThreadManager::ThreadManager(){

}

ThreadManager::~ThreadManager(){

	Join();
}

void ThreadManager::Launch(function<void(void)> callback){

	lock_guard<mutex> _lock(_threadsMutex);

	_threads.push_back(thread([=]() {
		LSendBufferPool = new PushLockBufferPool();
		LDBConnector = new DBConnector();
		callback();
	}));
}

void ThreadManager::Join(){

	for (thread& t : _threads) {
		if (t.joinable()) {
			t.join();
		}
	}
	_threads.clear();
}
