#include "pch.h"
#include "ThreadManager.h"

ThreadManager::ThreadManager(){

}

ThreadManager::~ThreadManager(){

	Join();
}

void ThreadManager::Launch(function<void(void)> callback){

	thread t(callback);
	_threads.push_back(t);
}

void ThreadManager::Join(){

	for (thread& t : _threads) {
		t.join();
	}
}
