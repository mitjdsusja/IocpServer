#pragma once
#include "pch.h"
#include "Vector.h"

struct GameStateData {
	wstring _name;
	Vector<int16> _position;
	Vector<int16> _velocity;
	Vector<int16> _rotation;
	uint64 _moveTimeStamp;
	bool _updatePosition;

	int32 _level;
};
struct RoomPlayerData {
	uint64 _sessionId;
	GameStateData _gameState;
};