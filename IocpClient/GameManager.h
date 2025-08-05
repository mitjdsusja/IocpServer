#pragma once

#include "Room.h"

class GameManager{
public:
	void RequestEnterRoomAllPlayer(int32 roomId);

	vector<RoomInfo> GetEnterableRoomList();
	int32 GetEnteredPlayerCount();

	void SetEnterableRoomList(const vector<RoomInfo>& roomList);
	void AddEnterPlayerCount();

private:
	vector<RoomInfo> _roomList;
	atomic<int32> _enteredPlayerCount = 0;
};

