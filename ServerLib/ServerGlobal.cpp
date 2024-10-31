#include "pch.h"
#include "ThreadManager.h"
#include "BufferPool.h"

ThreadManager* GThreadManager = nullptr;
//BufferPool* GSendBufferPool = nullptr;

class ServerGlobal {
public:
	ServerGlobal() {
		GThreadManager = new ThreadManager();
		//GSendBufferPool = new BufferPool();
	}

	~ServerGlobal() {
		delete GThreadManager;
		//delete GSendkBufferPool;
	}

}GServerGlobal;