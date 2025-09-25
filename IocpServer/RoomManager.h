#pragma once
#include "Vector.h"
#include "Actor.h"
#include "RoomInfo.h"
#include "RoomManagerResult.h"
#include "RoomPlayerData.h"

class GridManager;
class Job;

class Room : public Actor{
public:
	Room(const InitRoomInfo& initRoomInfo, const RoomPlayerData& hostPlayerData);
	~Room();

	void PushJobBroadcast(shared_ptr<Buffer> sendBuffer);
	void PushJobBroadcastPosition();
	void PushJobRegisterBroadcastPosition();
	void PushJobRegisterBroadcastPlayerInGrid();

	void PushJobEnterPlayer(const RoomPlayerData& initialPlayerData);
	void PushJobLeavePlayer(uint64 leavePlayerSessionId);
	void PushJobMovePlayer(uint64 movePlayerSessionId, const RoomPlayerData& roomPlayerData);

	void PushJobGetRoomInfo(function<void(RoomInfo& roomInfo)> func);
	void PushJobGetRoomPlayerList(function<void(vector<RoomPlayerData>)> func);

	int32 GetPlayerCount();

	void RegisterBroadcastMovement(uint32 reserveTime);
	void DestroyRoom();

public:
	void Broadcast(const shared_ptr<Buffer>& sendBuffer);
	void Broadcast(const vector<shared_ptr<Buffer>>& sendBuffer);
	void BroadcastPlayerMovement();
	void BroadcastPlayerInGrid();

	bool EnterPlayer(const RoomPlayerData& initialPlayerData);
	void LeavePlayer(uint64 sessionId);
	void MovePlayer(uint64 sessionId, const RoomPlayerData& roomPlayerData);

	RoomInfo GetRoomInfo();
	vector<RoomPlayerData> GetRoomPlayerList();

private:
	RoomInfo _roomInfo;

	bool _removeRoomFlag = false;

	shared_ptr<GridManager> _gridManager;

	map<uint64, RoomPlayerData> _players;
};

/*-----------------
	RoomManager
-------------------*/



class RoomManager : public Actor {
public:
	RoomManager(int32 maxRoomCount = 100);

	void PushJobCreateAndPushRoom(const InitRoomInfo& initRoomInfo, const RoomPlayerData& hostPlayerData);
	void PushJobEnterRoom(int32 roomid, const RoomPlayerData& enterPlayerData);
	void PushJobLeaveRoom(int32 roomId, uint64 sessionId);
	void PushJobRemoveRoom(int32 roomId);
	void PushJobMovePlayer(int32 roomId, const RoomPlayerData& roomPlayerData);
	void PushJobSkillUse();

	void PushJobGetRoomInfoList(function<void(vector<RoomInfo>)> func);
	void PushJobGetRoomPlayerList(int32 roomId, function<void(vector<RoomPlayerData>)> func);

	// Result
	void PushJobEnterRoomResult(const RoomManagerResult::EnterRoomResult enterRoomResult);

	void BroadcastToRoom(int32 roomId, shared_ptr<Buffer> sendBuffer);

public:
	int32 CreateAndPushRoom(const InitRoomInfo& initRoomInfo, const RoomPlayerData& hostPlayerData);
	bool EnterRoom(int32 roomId, const RoomPlayerData& enterPlayerData);
	void RemoveRoom(int32 roomId);
	void LeaveRoom(int32 roomid, uint64 sessionId);
	void MovePlayer(int32 roomId, const RoomPlayerData& roomPlayerData);

	void EnterRoomResult(const RoomManagerResult::EnterRoomResult enterRoomResult);

private:
	static shared_ptr<Room> MakeRoomPtr(const InitRoomInfo& initRoomInfo, const RoomPlayerData& hostPlayerData);

private:
	mutex _roomsMutex;

	int32 _maxRoomCount;
	atomic<int32> _nextRoomId = 1;
	map<int32, shared_ptr<Room>> _rooms;
	unordered_map<int64, int32> _sessionToRoomMap;
};

