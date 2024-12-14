#include "pch.h"
#include "ThreadManager.h"
#include "BufferPool.h"
#include "JobQueue.h"

ThreadManager* GThreadManager = nullptr;
JobQueue* GJobQueue = nullptr;

class ServerGlobal {
public:
	ServerGlobal() {
		GThreadManager = new ThreadManager();
		GJobQueue = new JobQueue();
	}

	~ServerGlobal() {
		delete GThreadManager;
		delete GJobQueue;
	}

}GServerGlobal;