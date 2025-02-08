#include "pch.h"
#include "Global.h"

#include "RoomManager.h"

RoomManager* GRoomManager = nullptr;

class Global {
public :
	Global() {
		GRoomManager = new RoomManager();
	}
	~Global() {
		delete GRoomManager;
	}
};