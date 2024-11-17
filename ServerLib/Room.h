#pragma once
#include "ServerPch.h"

class Room {
public:
	Room(int32 maxUserCount);

	void Enter(shared_ptr<Session*> session);
	void Exit(shared_ptr<Session*> session);

private:
	mutex _mutex;

	int32 _maxUserCount = 0;
	map<uint32, shared_ptr<Session*>> sessions;

};