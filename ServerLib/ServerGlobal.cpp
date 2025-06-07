#include "pch.h"
#include "ThreadManager.h"
#include "BufferPool.h"
#include "JobQueue.h"
#include "JobTimer.h"
#include "JobScheduler.h"

ThreadManager* GThreadManager = nullptr;
LockBufferPool* GSendBufferPool = nullptr;
JobScheduler* GJobScheduler = nullptr;

class ServerGlobal {
public:
	ServerGlobal() {
		GThreadManager = new ThreadManager();
		GSendBufferPool = new LockBufferPool();
		GJobScheduler = new JobScheduler();
	}

	~ServerGlobal() {
		delete GThreadManager;
		delete GSendBufferPool;
		delete GJobScheduler;
	}

}GServerGlobal;