#pragma once
#include "pch.h"
#include "Vector.h"

struct RoomPlayerBaseInfo {

	uint64 _sessionId = 0;
	wstring _name = L"";
};

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
	int32 _maxHp = 0;
	int32 _mp = 0;
	int32 _maxMp = 0;
	int64 _exp = 0;
	int64 _maxExp = 0;
};

struct RoomPlayerData {

	bool _enterRoomComplete = false;

	RoomPlayerBaseInfo _baseInfo;
	RoomPlayerTransform _transform;
	RoomPlayerStats _stats;
};