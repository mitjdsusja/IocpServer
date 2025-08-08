#pragma once
#include "Vector.h"

class RoomInfo;

struct PlayerInfo {

	Vector<int16> _position;
	Vector<int16> _velocity;
};

class Player{
public:
	Player(const shared_ptr<Session>& owner);

	void RandomMove();
	void SendData(const vector<shared_ptr<Buffer>>& sendBuffer);

	uint64 GetSessionId();

private:
	shared_ptr<Session> _owner;

	PlayerInfo _playerInfo;

};

class PlayerManager {
public:
	void CreatePlayerAndAdd(const shared_ptr<Session>& playerOwner, uint64  userId);
	void RequestRoomList();
	void RequestEnterRoomAllPlayer(uint64 roomId);
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

