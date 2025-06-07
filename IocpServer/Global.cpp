#include "pch.h"
#include "Global.h"

#include "RoomManager.h"
#include "PlayerManager.h"

shared_ptr<RoomManager> GRoomManager = nullptr;
shared_ptr<PlayerManager> GPlayerManager = nullptr;

class Global {
public :
	Global() {
		GRoomManager = make_shared<RoomManager>();
		GPlayerManager = make_shared<PlayerManager>();
	}
	~Global() {
		//delete GRoomManager;
		//delete GPlayerManager;
	}
}Global;