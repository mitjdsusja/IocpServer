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
	int32 GetId() { return _id; }
	Position& GetPosition() { return _position; }
	Direction& GetDirection() { return _direction; }

	void SetId(int32 id) { _id = id; }
	void SetPosition(float x, float y, float z) { _position.x = x; _position.y = y; _position.z = z; }
	void SetDirection(float x, float y, float z) { _direction.x = x; _direction.y = y; _direction.z = z; }

private:
	int32 _id;
	Position _position;
	Direction _direction;

};

