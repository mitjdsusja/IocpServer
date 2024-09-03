#include "pch.h"
#include "ServerGlobal.h"

ThreadManager* GThreadManager = nullptr;

class ServerGlobal {
public:
	ServerGlobal() {
		GThreadManager = new ThreadManager();
	}

	~ServerGlobal() {
		delete GThreadManager;
	}

}ServerGlobal;