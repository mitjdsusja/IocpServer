#include "pch.h"
#include "ThreadManager.h"
#include "BufferPool.h"
#include "JobQueue.h"
#include "JobTimer.h"

ThreadManager* GThreadManager = nullptr;
JobQueue* GJobQueue = nullptr;
JobTimer* GJobTimer = nullptr;

class ServerGlobal {
public:
	ServerGlobal() {
		GThreadManager = new ThreadManager();
		GJobQueue = new JobQueue();
		GJobTimer = new JobTimer();
	}

	~ServerGlobal() {
		delete GThreadManager;
		delete GJobQueue;
		delete GJobTimer;
	}

}GServerGlobal;