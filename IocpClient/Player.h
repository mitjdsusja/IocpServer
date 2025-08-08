#pragma once
#include "Vector.h"
#include <chrono>

class RoomInfo;

struct Acount {

	string _id;
	string _pw;
};

struct PlayerInfo {

	using TimePoint = chrono::system_clock::time_point;

	int32 _enterRoomId = 0;
	int16 _moveSpeed = 10;
	Vector<float> _position;
	Vector<float> _velocity;
	TimePoint _lastCalculatedTimePoint;
	float _moveTimeRemaining = 0.0f;
};

class Player{
public:
	Player(const shared_ptr<Session>& owner);

	void RandomMove();
	void SendData(const vector<shared_ptr<Buffer>>& sendBuffer);

	uint64 GetSessionId();
	PlayerInfo GetPlayerInfo();

private:
	shared_ptr<Session> _owner;

	PlayerInfo _playerInfo;
	Acount _acount;

};

class PlayerManager {
public:
	void CreatePlayerAndAdd(const shared_ptr<Session>& playerOwner, uint64  userId);
	void RequestRoomList();
	void AllPlayerRequestEnterRoom(uint64 roomId);
	void AllPlayerRandomMove();
	void AllPlayerSendMovePacket();
	void SendMsg(uint64 userId, vector<shared_ptr<Buffer>> sendBuffers);

	uint32 GetPlayerCount() { return _playerCount; }

private:
	shared_ptr<Player> CreatePlayer(const shared_ptr<Session>& playerOwner);
	void AddPlayer(const shared_ptr<Player>& player, uint64 userId);
	void PlayerRandomMove();

private:
	atomic<uint32> _playerCount = 0;
	map<uint64, shared_ptr<Player>> _players;
};

