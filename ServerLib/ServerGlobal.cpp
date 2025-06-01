#include "pch.h"
#include "ThreadManager.h"
#include "BufferPool.h"
#include "JobQueue.h"
#include "JobTimer.h"
#include "JobScheduler.h"

ThreadManager* GThreadManager = nullptr;
JobQueue* GJobQueue = nullptr;
JobTimer* GJobTimer = nullptr;
LockBufferPool* GSendBufferPool = nullptr;
JobScheduler* GJobScheduler = nullptr;

class ServerGlobal {
public:
	ServerGlobal() {
		GThreadManager = new ThreadManager();
		GJobQueue = new JobQueue();
		GJobTimer = new JobTimer();
		GSendBufferPool = new LockBufferPool();
		GJobScheduler = new JobScheduler();
	}

	~ServerGlobal() {
		delete GThreadManager;
		delete GJobQueue;
		delete GJobTimer;
		delete GSendBufferPool;
		delete GJobScheduler;
	}

}GServerGlobal;