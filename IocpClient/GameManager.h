#pragma once

#include "Room.h"

class GameManager{
public:
	void ReqeustEnterRoomAllPlayer();
	void RequestEnterRoomAllPlayer(int32 roomId);
	void ReqeustCreateRoom(int32 createRoomCount);
	void PlayerMovement();
	void SendPingPacket();

	vector<RoomInfo> GetEnterableRoomList();
	int32 GetEnteredPlayerCount();
	int32 GetEnterRoomId();
	uint64 GetNowServerTimeMs();
	uint64 GetNowClientTimeMs();

	void SetEnterableRoomList(const vector<RoomInfo>& roomList);
	void AddEnterPlayerCount();
	void SetEnterRoomId(int32 RoomId);
	void SetServerTimeOffsetMs(uint64 serverTimeOffsetMs);

public:
	atomic<int32> _createRoomPlayerSessionCount;

private:
	vector<RoomInfo> _roomList;
	int32 _enterRoomId = 0;
	atomic<int32> _enteredPlayerCount = 0;

	atomic<uint64> _serverTimeOffsetMs = 0;

};

