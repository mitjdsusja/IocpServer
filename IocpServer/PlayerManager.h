#pragma once
#include "Vector.h"
#include "Actor.h"
#include "RoomManager.h"

class GameSession;

struct PlayerBaseInfo {
	uint64 _sessionId = 0;
	wstring _name = L"";
};
struct PlayerPosition {
	int32 _roomId = 0;
	Vector<int16> _position;
	Vector<int16> _velocity;
	int64 _moveTimestamp = 0;
};
struct PlayerStats {
	int32 _level = 0;
	
};
struct PlayerInfo {
	PlayerBaseInfo _baseInfo;
	PlayerPosition _position;
	PlayerStats _stats;
};

class Player : public Actor{
public:
	Player(shared_ptr<GameSession> owner);
	~Player();

	void InitPlayer(const PlayerBaseInfo& baseInfo, const PlayerPosition& position, const PlayerStats& stats);

	void PushJobSendData(const shared_ptr<Buffer>& sendBuffer);
	void PushJobUpdatePosition(const PlayerPosition& newPosition);

	void PushJobGetBaseInfo(function<void(PlayerBaseInfo)> func);
	void PushJobGetPosition(function<void(PlayerPosition)> func);
	void PushJobGetStats(function<void(PlayerStats)> func);

	void PushJobSetPosition(const PlayerPosition& position);

	shared_ptr<GameSession>& GetOwnerSession() { return _owner; }

public:
	// 외부에서 절대 사용 금지
	void SendData(const shared_ptr<Buffer>& sendBuffer);
	void UpdatePosition(const PlayerPosition& newPosition);

	PlayerBaseInfo GetBaseInfo();
	PlayerPosition GetPosition();
	PlayerStats GetStats();

	void SetPosition(const PlayerPosition& position);

private:
	void ClearResource();

private:
	shared_ptr<GameSession> _owner = nullptr;
	mutex _playerMutex;

	PlayerInfo _info;
};

class PlayerManager : public Actor {
public:
	PlayerManager();
	~PlayerManager();

	void PushJobSendData(uint64 sessionId, const shared_ptr<Buffer>& sendBuffer);
	void PushJobSendData(uint64 sessionId, const vector<shared_ptr<Buffer>>& sendBuffer);
	void PushJobCreateAndPushPlayer(const shared_ptr<GameSession>& ownerSession, const PlayerBaseInfo& baseInfo, const PlayerPosition& position, const PlayerStats& stats);
	void PushJobRemovePlayer(uint64 sessionId);

	void PushJobGetRoomPlayer(uint64 sessionId, function<void(PlayerBaseInfo, PlayerPosition)>);
	void PushJobGetBaseInfo(uint64 sessionId, function<void(PlayerBaseInfo)> func);
	void PushJobGetPosition(uint64 sessionId, function<void(PlayerPosition)> func);
	void PushJobGetstats(uint64 sessionId, function<void(PlayerStats)> func);

	void PushJobSetPosition(uint64 sessionId, PlayerPosition position);

public:
	// 외부에서 절대 사용 금지
	void SendData(uint64 sessionId, const shared_ptr<Buffer>& sendBuffer);
	void CreateAndPushPlayer(const shared_ptr<GameSession>& ownerSession, const PlayerBaseInfo& baseInfo, const PlayerPosition& position, const PlayerStats& stats);
	void RemovePlayer(uint64 sessionId);

	void SetPosition(uint64 sessionId, const PlayerPosition& position);

private:
	mutex _playersMutex;

	map<uint64, shared_ptr<Player>> _players;

};

