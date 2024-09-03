#include "pch.h"
#include "ServerGlobal.h"

class ThreadManager* GThreadManager = nullptr;

class ServerGlobal {
public:
	ServerGlobal() {
		GThreadManager = new ThreadManager();
	}

	~ServerGlobal() {
		delete GThreadManager;
	}

}ServerGlobal;