#pragma once
#include "ServerPch.h"

#pragma pack(push, 1)
struct Position {
	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
};

struct Direction {
	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
};
#pragma pack(pop)

class UserInfo {
public:
	UserInfo() {};
	UserInfo(shared_ptr<Session> owner) : _owner(owner) {};

	shared_ptr<Session>& GetOwner() { return _owner; };

	int32 GetId() { return _id; }
	Position& GetPosition() { return _position; }
	Direction& GetDirection() { return _direction; }
	int64 GetLastMovePacket() { return _lastMovePacket; }

	void SetId(int32 id) { _id = id; }
	void SetPosition(int32 x, int32 y, int32 z) { _position.x = x; _position.y = y; _position.z = z; }
	void SetDirection(int32 x, int32 y, int32 z) { _direction.x = x; _direction.y = y; _direction.z = z; }
	void SetLastMovePacket(int64 time) { _lastMovePacket = time; }

private:
	shared_ptr<Session> _owner = nullptr;

	int32 _id = 0;
	Position _position = {};
	Direction _direction = {};
	int32 _moveSpeed = 0;
	int64 _lastMovePacket = 0;

};

