#pragma once
#include "ServerPch.h"

struct Position {
	float x = 0;
	float y = 0;
	float z = 0;
};

struct Direction {
	float x = 0;
	float y = 0;
	float z = 0;
};

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
	void SetPosition(float x, float y, float z) { _position.x = x; _position.y = y; _position.z = z; }
	void SetDirection(float x, float y, float z) { _direction.x = x; _direction.y = y; _direction.z = z; }
	void SetLastMovePacket(int64 time) { _lastMovePacket = time; }

private:
	shared_ptr<Session> _owner = nullptr;

	int32 _id = 0;
	Position _position = {};
	Direction _direction = {};
	float _moveSpeed = 0;
	int64 _lastMovePacket = 0;

};

