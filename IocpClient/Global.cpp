#include "pch.h"
#include "Global.h"

#include "Player.h"

shared_ptr<PlayerManager> GPlayerManager = nullptr;

class Global {
public:
	Global() {
		GPlayerManager = make_shared<PlayerManager>();

	}
	
	~Global() {

	}

}Global;