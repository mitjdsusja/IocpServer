#pragma once
#include <pch.h>

#include "PlayerManager.h"

struct RoomInfo {
	int32 _roomId;
	int32 _maxPlayerCount;
	int32 _curPlayerCount;
	wstring _roomName;
	wstring _hostPlayerName;
	vector<PlayerInfo> _playerInfoList;
};

class Room {
public:
	Room(int32 roomId, shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount);
	~Room();

	void Broadcast(shared_ptr<Buffer> sendBuffer);

	void AddPlayer(uint64 sessionId, shared_ptr<Player> player);
	void RemovePlayer(uint64 sessionId);

	RoomInfo GetRoomInfo();
	int32 GetPlayerCount();

	vector<PlayerInfo> GetRoomPlayerInfoList(int32 roomId);

private:
	mutex _roomMutex;

	int32 _roomId = 0;
	int32 _maxPlayerCount = 0;
	int32 _curPlayerCount = 0;
	wstring _roomName = L"NULL";
	shared_ptr<Player> _hostPlayer = nullptr;
	map<uint64, shared_ptr<Player>> _players;

};

class RoomManager{
public:
	RoomManager(int32 maxRoomCount = 100);

	void BroadcastToRoom(int32 roomId, shared_ptr<Buffer> sendBuffer);
	
	int32 CreateAndAddRoom(shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount = 10);
	bool EnterRoom(int32 roomId,int64 sessionid, shared_ptr<Player> player);
	void RemoveRoom(int32 roomId);
	void RemovePlayerFromRoom(int32 roomid, uint64 sessionId);

	vector<RoomInfo> GetRoomInfoList();
	RoomInfo GetRoomInfo(int32 roomId);

private:
	static shared_ptr<Room> MakeRoomPtr(int32 roomId, shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount);

private:
	mutex _roomsMutex;

	int32 _maxRoomCount;
	atomic<int32> _nextRoomId = 1;
	map<int32, shared_ptr<Room>> _rooms;

};

