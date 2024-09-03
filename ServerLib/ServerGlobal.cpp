#include "pch.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr;

class ServerGlobal {
public:
	ServerGlobal() {
		GThreadManager = new ThreadManager();
	}

	~ServerGlobal() {
		delete GThreadManager;
	}

}GServerGlobal;