#pragma once
#include "Vector.h"
#include "JobQueue.h"

class GridManager;
class Job;

struct InitRoomInfo {
	int32 _roomId;
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

class Room : public JobQueueBase{
public:
	struct GameStateData {
		wstring _name;
		Vector<int16> _position;
		Vector<int16> _velocity;
		uint64 _moveTimeStamp;
		bool _updatePosition;
	};
	struct RoomPlayer {
		uint64 _sessionId;
		GameStateData _gameState;
	};
public:
	Room(const InitRoomInfo& initRoomInfo, const RoomPlayer& hostPlayerData);
	~Room();

	void PushJobBroadcast(shared_ptr<Buffer> sendBuffer);
	void BroadcastPlayerMovement();

	void PushJobEnterPlayer(uint64 enterPlayerSessionId, const RoomPlayer& initialPlayerData);
	void PushJobLeavePlayer(uint64 leavePlayerSessionId);
	void PushJobMovePlayer(uint64 movePlayerSessionId, const Room::RoomPlayer& roomPlayerData);

	void PushJobGetRoomInfo(function<void(RoomInfo& roomInfo)> func);
	void PushJobGetRoomPlayerList(function<void(vector<Room::RoomPlayer>)> func);

	int32 GetPlayerCount();

	void RegisterBroadcastMovement(uint32 reserveTime);
	void DestroyRoom();

public:
	void Broadcast(const shared_ptr<Buffer>& sendBuffer);

	bool EnterPlayer(uint64 enterPlayerSessionId, const RoomPlayer& initialPlayerData);
	void LeavePlayer(uint64 sessionId);
	void MovePlayer(uint64 sessionId, const Room::RoomPlayer& roomPlayerData);

	RoomInfo GetRoomInfo();
	vector<Room::RoomPlayer> GetRoomPlayerList();


private:
	RoomInfo _roomInfo;

	bool _removeRoomFlag = false;

	shared_ptr<GridManager> _gridManager;

	map<uint64, RoomPlayer> _players;
};

/*-----------------
	RoomManager
-------------------*/

class RoomManager : public JobQueueBase {
public:
	RoomManager(int32 maxRoomCount = 100);

	void PushJobCreateAndPushRoom(const InitRoomInfo& initRoomInfo, const Room::RoomPlayer& hostPlayerData);
	void PushJobEnterRoom(int32 roomid, const Room::RoomPlayer& enterPlayerData);
	void PushJobLeaveRoom(int32 roomId, uint64 sessionId);
	void PushJobRemoveRoom(int32 roomId);
	void PushJobMovePlayer(int32 roomId, const Room::RoomPlayer& roomPlayerData);

	void PushJobGetRoomInfoList(function<void(vector<RoomInfo>)> func);
	void PushJobGetRoomPlayerList(int32 roomId, function<void(vector<Room::RoomPlayer>)> func);

	void BroadcastToRoom(int32 roomId, shared_ptr<Buffer> sendBuffer);

public:
	int32 CreateAndPushRoom(const InitRoomInfo& initRoomInfo, const Room::RoomPlayer& hostPlayerData);
	bool EnterRoom(int32 roomId, const Room::RoomPlayer& enterPlayerData);
	void RemoveRoom(int32 roomId);
	void LeaveRoom(int32 roomid, uint64 sessionId);
	void MovePlayer(int32 roomId, const Room::RoomPlayer& roomPlayerData);

private:
	static shared_ptr<Room> MakeRoomPtr(const InitRoomInfo& initRoomInfo, const Room::RoomPlayer& hostPlayerData);

private:
	mutex _roomsMutex;

	int32 _maxRoomCount;
	atomic<int32> _nextRoomId = 1;
	map<int32, shared_ptr<Room>> _rooms;

};

