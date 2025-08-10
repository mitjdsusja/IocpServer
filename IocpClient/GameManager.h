#pragma once

#include "Room.h"

class GameManager{
public:
	void RequestEnterRoomAllPlayer(int32 roomId);
	void PlayerMovement();
	void SendPingPacket();

	vector<RoomInfo> GetEnterableRoomList();
	int32 GetEnteredPlayerCount();
	int32 GetEnterRoomId();
	uint64 GetNowServerTimeMs();

	void SetEnterableRoomList(const vector<RoomInfo>& roomList);
	void AddEnterPlayerCount();
	void SetEnterRoomId(int32 RoomId);
	void SetServerTimeOffsetMs(uint64 serverTimeOffsetMs);

private:
	vector<RoomInfo> _roomList;
	int32 _enterRoomId = 0;
	atomic<int32> _enteredPlayerCount = 0;

	atomic<uint64> _serverTimeOffsetMs = 0;
};

