#pragma once
#include "ServerPch.h"

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

class UserInfo {
public:
	int32 GetId() { return _id; }
	Position& GetPosition() { return _position; }
	Velocity& GetVelocity() { return _velocity; }

	void SetId(int32 id) { _id = id; }
	void SetPosition(float x, float y, float z) { _position.x = x; _position.y = y; _position.z = z; }
	void SetVelocity(float x, float y, float z) { _velocity.x = x; _velocity.y = y; _velocity.z = z; }

private:
	int32 _id;
	Position _position;
	Velocity _velocity;

};

