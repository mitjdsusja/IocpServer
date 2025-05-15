#pragma once
#include "Vector.h"

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
	Player(shared_ptr<GameSession> owner, PlayerInfo playerInfo);
	~Player();

	void ClearResource();

	shared_ptr<GameSession> GetOwner() { return _owner; }

	PlayerInfo GetPlayerInfo();

	void SetPlayerMove(PlayerInfo& playerInfo);

	void SetJoinedRoom(shared_ptr<Room> room);
	void SetPlayerInfo(PlayerInfo& playerInfo);
	void SetName(wstring& name);
	void SetRoomId(int32 roomId);
	void SetPosition(Vector<int16>& position);
	void SetVelocity(Vector<int16>& velocity);
	void SetMoveTimestamp(int64 timestamp);

	wstring GetName() { return _playerInfo._name; }
	int32 GetRoomId() { return _playerInfo._roomId; }

private:
	shared_ptr<GameSession> _owner = nullptr;
	mutex _playerMutex;

	shared_ptr<Room> _joinedRoom;
	PlayerInfo _playerInfo = {};
};

class PlayerManager {
public:
	PlayerManager();
	~PlayerManager();

	void CreateAndAddPlayer(shared_ptr<GameSession> owner, uint64 sessionId);
	void CreateAndAddPlayer(shared_ptr<GameSession> owner, uint64 sessionId, PlayerInfo playerInfo);
	shared_ptr<Player> GetPlayer(uint64 sessionId);
	void SetPlayerInfo(int64 sessionId, PlayerInfo& playerInfo);

	void RemovePlayer(uint64 sessionId);

private:
	mutex _playersMutex;
	map<uint64, shared_ptr<Player>> _players;

};

