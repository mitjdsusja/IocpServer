#pragma once
#include "Vector.h"

class GameSession;

struct PlayerInfo {
	wstring _name;
	int32 _level;
	int32 _roomId;
	Vector _position;
};

class Player{
public:
	Player(shared_ptr<GameSession> owner, wstring name, Vector position);
	~Player();

	void ClearResource();

	shared_ptr<GameSession> GetOwner() { return _owner; }

	PlayerInfo GetPlayerInfo();

	void SetPlayerMove(Vector& position, int64 timestamp);

	void SetPlayerInfo(PlayerInfo& playerInfo);
	void SetName(wstring& name);
	void SetRoomId(int32 roomId);
	void SetPosition(Vector& position);
	void SetMoveTimestamp(int64 timestamp);

	wstring GetName() { return _name; }
	int32 GetRoomId() { return _roomId; }

private:
	shared_ptr<GameSession> _owner = nullptr;
	mutex _playerMutex;

	wstring _name = L"";
	int32 _level = 0;
	int32 _roomId = 0;
	Vector _position;
	int64 _moveTimestamp = 0;

};

class PlayerManager {
public:
	PlayerManager();
	~PlayerManager();

	// TODO : 매개변수로 각각 값을 주지않고 PlayerInfo로 넘겨주기
	void CreateAndAddPlayer(shared_ptr<GameSession> owner, uint64 sessionId, wstring name, Vector position);
	shared_ptr<Player> GetPlayer(uint64 sessionId);
	void SetPlayerInfo(int64 sessionId, PlayerInfo& playerInfo);

	void RemovePlayer(uint64 sessionId);

private:
	mutex _playersMutex;
	map<uint64, shared_ptr<Player>> _players;

};

