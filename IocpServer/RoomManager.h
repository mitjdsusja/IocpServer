#pragma once
#include <pch.h>

#include "GridManager.h"
#include "JobQueue.h"

struct RoomInfo {
	int32 _roomId;
	int32 _maxPlayerCount;
	int32 _curPlayerCount;
	wstring _roomName;
	wstring _hostPlayerName;
	uint64 _hostPlayerSessionId;
	vector<uint64> _playersSessionId;
};

class Room : public JobQueueBase {
public:
	struct RoomPlayer {
		uint64 _sessionId;
		Player::GameStateData _gameState;
	};
public:
	Room(int32 roomId, wstring roomName, int32 maxPlayerCount, RoomPlayer hostPlayerData);
	~Room();

	void Broadcast(shared_ptr<Buffer> sendBuffer);

	void AddPlayer(uint64 sessionId, RoomPlayer playerData);
	void RemovePlayer(uint64 sessionId);
	void MovePlayer(uint64 sessionId, Vector<int16> newPosition);

	void BroadcastPlayerMovement();

	RoomInfo GetRoomInfo();
	int32 GetPlayerCount();

	void RegisterBroadcastMovement(uint32 reserveTime);
	void DestroyRoom();

private:
	mutex _roomMutex;

	shared_ptr<GridManager> _gridManager;

	int32 _roomId = 0;
	int32 _maxPlayerCount = 0;
	int32 _curPlayerCount = 0;
	wstring _roomName = L"NULL";
	uint64 _hostPlayerSessionId = 0;
	map<uint64, RoomPlayer> _players;

	bool _removeRoomFlag = false;

};

class RoomManager{
public:
	RoomManager(int32 maxRoomCount = 100);

	void BroadcastToRoom(int32 roomId, shared_ptr<Buffer> sendBuffer);
	
	int32 CreateAndAddRoom(shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount = 100);
	bool EnterRoom(int32 roomId,int64 sessionid, Room::RoomPlayer enterPlayerData);
	void RemoveRoom(int32 roomId);
	void LeavePlayerFromRoom(int32 roomid, uint64 sessionId);

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

