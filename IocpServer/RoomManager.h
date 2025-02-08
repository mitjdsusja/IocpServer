#pragma once
#include "Player.h"

struct RoomInfo {
	int32 _roomId;
	int32 _maxPlayerCount;
	int32 _curPlayerCount;
	wstring _roomName;
};

class Room {
public:
	Room(int32 roomId, Player* hostPlayer, wstring roomName, int32 maxPlayerCount);

	void AddPlayer(int32 playerId, Player* player);
	void RemovePlayer(int32 playerId);
	RoomInfo GetRoomInfo();

private:
	mutex _roomMutex;

	int32 _roomId;
	int32 _maxPlayerCount;
	int32 _curPlayerCount;
	wstring _roomName;
	Player* _hostPlayer;
	map<int32, Player*> _players;

};

class RoomManager{
public:
	void CreateAndAddRoom(Player* hostPlayer, wstring roomName, int32 maxPlayerCount = 10);
	
	vector<RoomInfo> GetRoomInfoList();

private:
	mutex _roomsMutex;

	int32 _maxRoomCount;
	atomic<int32> _nextRoomId = 1;
	map<int32, shared_ptr<Room>> _rooms;

};

