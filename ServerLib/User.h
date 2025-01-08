#pragma once
#include "ServerPch.h"

#pragma pack(push, 1)
struct Position {
	float x = 0;
	float y = 0;
	float z = 0;
};

struct Velocity {
	float x = 0;
	float y = 0;
	float z = 0;
};
#pragma pack(pop)

class UserInfo {
public:
	UserInfo() {};
	UserInfo(shared_ptr<Session> owner) : _owner(owner) {};

	shared_ptr<Session>& GetOwner() { return _owner; };

	int32 GetId() { return _id; }
	Position& GetPosition() { return _position; }
	Velocity& GetVelocity() { return _velocity; }
	int64 GetLastMovePacket() { return _lastMovePacket; }

	void SetId(int32 id) { _id = id; }
	void SetPosition(float x, float y, float z) { _position.x = x; _position.y = y; _position.z = z; }
	void SetVelocity(float x, float y, float z) { _velocity.x = x; _velocity.y = y; _velocity.z = z; }
	void SetLastMovePacket(int64 time) { _lastMovePacket = time; }

private:
	shared_ptr<Session> _owner = nullptr;

	int32 _id = 0;
	Position _position = {};
	Velocity _velocity = {};
	float _moveSpeed = 0;
	int64 _lastMovePacket = 0;

};

