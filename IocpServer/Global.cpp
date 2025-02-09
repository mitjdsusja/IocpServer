#include "pch.h"
#include "Global.h"

#include "RoomManager.h"
#include "PlayerManager.h"

RoomManager* GRoomManager = nullptr;
PlayerManager* GPlayerManager = nullptr;

class Global {
public :
	Global() {
		GRoomManager = new RoomManager();
		GPlayerManager = new PlayerManager();
	}
	~Global() {
		delete GRoomManager;
		delete GPlayerManager;
	}
}Global;