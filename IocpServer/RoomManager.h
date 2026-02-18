#pragma once
#include "Vector.h"
#include "Actor.h"
#include "RoomInfo.h"
#include "RoomResult.h"
#include "RoomPlayerData.h"
#include "SkillData.h"
#include "MapLoader.h"

class GridManager;
class Job;

class Room : public Actor{
public:
	Room(const InitRoomInfo& initRoomInfo, const RoomPlayerData& hostPlayerData);
	~Room();

	void PushJobBroadcast(const shared_ptr<Buffer>& sendBuffer);
	void PushJobBroadcastPosition();
	void PushJobBroadcastNearByPlayer(uint64 sessionId, const shared_ptr<Buffer>& sendBuffer);
	void PushJobRegisterBroadcastPosition();
	void PushJobRegisterBroadcastPlayerInGrid();

	void PushJobEnterPlayer(const RoomPlayerData& initialPlayerData, function<void(const RoomResult::EnterRoomResult&)> callback);
	void PushJobEnterRoomComplete(uint64 sessionId);
	void PushJobLeavePlayer(uint64 leavePlayerSessionId);
	void PushJobMovePlayer(uint64 movePlayerSessionId, const RoomPlayerTransform& roomPlayerTransform);

	void PushJobSkillUse(const SkillData& skillData, function<void(const RoomResult::SkillUseResult&)> callback);

	void PushJobGetRoomInfo(function<void(RoomInfo& roomInfo)> func);
	void PushJobGetRoomPlayerList(function<void(vector<RoomPlayerData>)> func);

	int32 GetPlayerCount();

	void RegisterBroadcastMovement(uint32 reserveTime);
	void DestroyRoom();

public:
	void Broadcast(const shared_ptr<Buffer>& sendBuffer);
	void BroadcastPlayerLeaveGrid(uint64 sessionId, const vector<uint64>& playersToNotify);
	void BroadcastPlayerEnterGrid(uint64 sessionId, const vector<uint64>& playersToNotify);
	void BroadcastPlayerMovement();
	void BroadcastPlayerInGrid();
	void SendPlayersInGrid(uint64 sesssionId);

	bool EnterPlayer(const RoomPlayerData& initialPlayerData);
	void EnterRoomComplete(uint64 sessionId);
	void LeavePlayer(uint64 sessionId);
	void MovePlayer(uint64 sessionId, const RoomPlayerTransform& roomPlayerTransform);
	void NotifyGridChange(uint64 sessionId, const Vector<int32>& oldCell, const Vector<int32>& newCell);

	RoomResult::SkillUseResult SkillUse(const SkillData& skillData);

	RoomInfo GetRoomInfo();
	RoomPlayerData GetRoomPlayerData(uint64 sessionId);
	vector<RoomPlayerData> GetRoomPlayerList();

private:
	RoomInfo _roomInfo;

	bool _removeRoomFlag = false;

	shared_ptr<GridManager> _gridManager;

	unordered_map<uint64, RoomPlayerData> _players;
	const MapData* _mapData;
};

/*-----------------
	RoomManager
-------------------*/



class RoomManager : public Actor {
public:
	RoomManager(int32 maxRoomCount = 100);

	void PushJobCreateAndPushRoom(const InitRoomInfo& initRoomInfo, const RoomPlayerData& hostPlayerData);
	void PushJobEnterRoom(int32 roomid, const RoomPlayerData& enterPlayerData);
	void PushJobEnterRoomComplete(uint64 sessionId);
	void PushJobLeaveRoom(int32 roomId, uint64 sessionId);
	void PushJobRemoveRoom(int32 roomId);
	void PushJobMovePlayer(uint64 playerId, const RoomPlayerTransform& roomPlayerTransform);
	void PushJobSkillUse(const SkillData& skillData);

	void PushJobGetRoomInfoList(function<void(const vector<RoomInfo>&)> func);
	void PushJobGetRoomPlayerList(int32 roomId, function<void(vector<RoomPlayerData>)> func);

	// Result
	void PushJobEnterRoomResult(const RoomResult::EnterRoomResult& enterRoomResult);
	void PushJobSkillUseResult(const RoomResult::SkillUseResult& skillUseResult);

	void BroadcastToRoom(int32 roomId, const shared_ptr<Buffer>& sendBuffer);

public:
	int32 CreateAndPushRoom(const InitRoomInfo& initRoomInfo, const RoomPlayerData& hostPlayerData);
	bool EnterRoom(int32 roomId, const RoomPlayerData& enterPlayerData);
	void EnterRoomComplete(uint64 sessionId);
	void RemoveRoom(int32 roomId);
	void LeaveRoom(int32 roomid, uint64 sessionId);
	void MovePlayer(uint64 playerId, const RoomPlayerTransform& roomPlayerTransform);
	void SkillUse(const SkillData& skillData);

	void EnterRoomResult(const RoomResult::EnterRoomResult& enterRoomResult);
	void SkillUseResult(const RoomResult::SkillUseResult& skillUseResult);

private:
	static shared_ptr<Room> MakeRoomPtr(const InitRoomInfo& initRoomInfo, const RoomPlayerData& hostPlayerData);

private:
	mutex _roomsMutex;

	int32 _maxRoomCount;
	atomic<int32> _nextRoomId = 1;
	unordered_map<int32, shared_ptr<Room>> _rooms;
	unordered_map<int64, int32> _sessionToRoomMap;
};

