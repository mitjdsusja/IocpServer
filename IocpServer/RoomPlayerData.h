#pragma once
#include "pch.h"
#include "Vector.h"

struct RoomPlayerTransform {
	
	Vector<int16> _position;
	Vector<int16> _velocity;
	Vector<int16> _rotation;
	uint64 _moveTimeStamp;
	bool _updatePosition;
};

struct RoomPlayerStats {

	int32 _level = 0;
	int32 _hp = 0;
	int32 _mp = 0;
	int32 _exp = 0;
};

struct RoomPlayerData {

	uint64 _sessionId = 0;
	wstring _name = L"";

	bool _enterRoomComplete = false;

	RoomPlayerTransform _transform;
	RoomPlayerStats _stats;
};