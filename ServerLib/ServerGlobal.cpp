#include "pch.h"
#include "ThreadManager.h"
#include "BufferPool.h"

ThreadManager* GThreadManager = nullptr;
LockBufferPool* GSendBufferPool = nullptr;

class ServerGlobal {
public:
	ServerGlobal() {
		GThreadManager = new ThreadManager();
		GSendBufferPool = new LockBufferPool();
	}

	~ServerGlobal() {
		delete GThreadManager;
		delete GSendBufferPool;
	}

}GServerGlobal;