#pragma once
#include "Vector.h"
#include "Actor.h"
#include "RoomManager.h"
#include "PlayerData.h"

class GameSession;

class Player : public Actor{
public:
	Player(shared_ptr<GameSession> owner);
	~Player();

	void InitPlayer(const PlayerData& playerData);

	void PushJobSendData(const shared_ptr<Buffer>& sendBuffer);
	void PushJobUpdatePosition(const PlayerTransform& newPosition);

	void PushJobGetPlayerData(function<void(const PlayerData& playerData)> func);
	void PushJobGetBaseInfo(function<void(PlayerBaseInfo)> func);
	void PushJobGetPosition(function<void(PlayerTransform)> func);
	void PushJobGetStats(function<void(PlayerStats)> func);

	void PushJobSetTransform(const PlayerTransform& transform);

	shared_ptr<GameSession>& GetOwnerSession() { return _owner; }

private:
	void SendData(const shared_ptr<Buffer>& sendBuffer);
	void UpdateTransform(const PlayerTransform& newPosition);

	PlayerBaseInfo GetBaseInfo();
	PlayerTransform GetTransform();
	PlayerStats GetStats();

	void SetTransform(const PlayerTransform& transform);

private:
	void ClearResource();

private:
	shared_ptr<GameSession> _owner = nullptr;

	PlayerData _playerData;
};

class PlayerManager : public Actor {
public:
	PlayerManager();
	~PlayerManager();

	void PushJobSendData(uint64 sessionId, const shared_ptr<Buffer>& sendBuffer);
	void PushJobCreateAndPushPlayer(const shared_ptr<GameSession>& ownerSession, const PlayerData& playerData);
	void PushJobRemovePlayer(uint64 sessionId);

	void PushJobGetPlayerData(uint64 sessionId, function<void(const PlayerData& playerData)>);
	void PushJobGetRoomPlayer(uint64 sessionId, function<void(PlayerBaseInfo, PlayerTransform)>);
	void PushJobGetBaseInfo(uint64 sessionId, function<void(PlayerBaseInfo)> func);
	void PushJobGetPosition(uint64 sessionId, function<void(PlayerTransform)> func);
	void PushJobGetstats(uint64 sessionId, function<void(PlayerStats)> func);

	void PushJobSetTransform(uint64 sessionId, const PlayerTransform& transform);

private:
	void SendData(uint64 sessionId, const shared_ptr<Buffer>& sendBuffer);
	void CreateAndPushPlayer(const shared_ptr<GameSession>& ownerSession, const PlayerData& playerData);
	void RemovePlayer(uint64 sessionId);

	void SetTransform(uint64 sessionId, const PlayerTransform& position);

private:
	map<uint64, shared_ptr<Player>> _players;

};

