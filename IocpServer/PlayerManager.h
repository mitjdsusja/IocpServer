#pragma once
#include "Vector.h"
#include "JobQueue.h"

class GameSession;
class Room;

struct PlayerInfo {
	wstring _name = L"";
	int32 _level = 0;
	int32 _roomId = 0;
	Vector<int16> _position;
	Vector<int16> _velocity;
	int64 _moveTimestamp = 0;
	bool _isInfoUpdated = true;
};

class Player{
public:
	struct GameStateData {
		wstring _name;
		Vector<int16> _position;
		Vector<int16> _velocity;
		uint64 _moveTimeStamp;
		bool _updatePosition;
	};
public:
	Player(shared_ptr<GameSession> owner, PlayerInfo& playerInfo);
	~Player();

	shared_ptr<GameSession> GetOwnerSession() { return _owner; }
	PlayerInfo GetPlayerInfo();

	void UpdatePlayerInfo(const PlayerInfo& newInfo);
	void SetName(const wstring& name);
	void SetLevel(int32 level);
	void SetRoomId(int32 roomId);
	void SetPosition(Vector<int16>& position);
	void SetVelocity(Vector<int16>& velocity);
	void SetMoveTimestamp(int64 timestamp);
	void UpdateGameState(const GameStateData& gameState);

private:
	void ClearResource();

private:
	shared_ptr<GameSession> _owner = nullptr;
	mutex _playerMutex;

	PlayerInfo _playerInfo = {};
};

class PlayerManager : public JobQueueBase {
public:
	PlayerManager();
	~PlayerManager();

	void PushCreatePlayerJob(shared_ptr<GameSession> ownerSession, uint64 sessionId, PlayerInfo playerInfo);
	void PushRemovePlayerJob(uint64 sessionId);
	void PushUpdatePlayerGameStateJob(uint64 sessionId, const Player::GameStateData& gameState);

private:
	mutex _playersMutex;
	map<uint64, shared_ptr<Player>> _players;

};

