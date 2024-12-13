#pragma once
#include "ServerPch.h"

struct Position {
	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
};

struct Velocity {
	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
};

class UserInfo {
public:
	int32 GetId() { return _id; }
	Position& GetPosition() { return _position; }
	Velocity& GetVelocity() { return _velocity; }

	void SetId(int32 id) { _id = id; }
	void SetPosition(int32 x, int32 y, int32 z) { _position.x = x; _position.y = y; _position.z = z; }
	void SetVelocity(int32 x, int32 y, int32 z) { _velocity.x = x; _velocity.y = y; _velocity.z = z; }

private:
	int32 _id;
	Position _position;
	Velocity _velocity;

};

