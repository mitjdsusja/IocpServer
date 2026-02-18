#pragma once
#include "Vector.h"
#include <chrono>

class RoomInfo;

struct Acount {

	string _id;
	string _pw;
};

struct PlayerData {

	using TimePoint = chrono::system_clock::time_point;

	string _name;

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
	void SendData(const shared_ptr<Buffer>& sendBuffer);

	uint64 GetSessionId();
	PlayerData GetPlayerInfo();

	void SetName(string name);
	void SetEnterRoomId(int32 roomId);

private:
	shared_ptr<Session> _owner;

	PlayerData _playerInfo;
	Acount _acount;

};

class PlayerManager {
public:
	void CreatePlayerAndAdd(const shared_ptr<Session>& playerOwner, uint64  userId);
	void RequestRoomList();
	void RequestEnterRoom(uint64 sessionId, uint32 roomId);
	void RequestCreateRoom(int32 createRoomCount);
	void AllPlayerRequestEnterRoom(uint32 roomId);
	void AllPlayerRandomMove();
	void AllPlayerSendMovePacket();
	void SendPingPacketToFirstPlayer();
	void SendMsg(uint64 userId, shared_ptr<Buffer> sendBuffer);

	uint32 GetPlayerCount() { return _playerCount; }

	void SetEnterRoomId(uint64 userId, int32 roomI);

private:
	shared_ptr<Player> CreatePlayer(const shared_ptr<Session>& playerOwner);
	void AddPlayer(const shared_ptr<Player>& player, uint64 userId);
	void PlayerRandomMove();

private:
	mutex _playersMutex;
	atomic<uint32> _playerCount = 0;
	map<uint64, shared_ptr<Player>> _players;
};

