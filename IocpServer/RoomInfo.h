#pragma once
#include "pch.h"

struct InitRoomInfo {
	int32 _roomId;
	int32 _mapId;
	int32 _maxPlayerCount;
	wstring _roomName;
};

struct RoomInfo {
	InitRoomInfo _initRoomInfo;
	int32 _curPlayerCount;
	wstring _hostPlayerName;
	uint64 _hostPlayerSessionId;
	vector<uint64> _playersSessionId;
};