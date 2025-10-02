#pragma once
#include "pch.h"
#include "Vector.h"

struct PlayerBaseInfo {
	uint64 _sessionId = 0;
	wstring _name = L"";
};
struct PlayerTransform {
	int32 _roomId = 0;
	Vector<int16> _position;
	Vector<int16> _rotation;
	Vector<int16> _velocity;
	int64 _lastmoveTimestamp = 0;
};
struct PlayerStats {
	int32 _level = 0;
	int32 _hp = 0;
	int32 _maxHp = 0;
	int32 _mp = 0;
	int32 _maxMp = 0;
	int32 _exp = 0;
	int32 _maxExp = 0;
};
struct PlayerData {
	PlayerBaseInfo _baseInfo;
	PlayerTransform _position;
	PlayerStats _stats;
};