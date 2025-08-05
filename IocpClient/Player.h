#pragma once

class RoomInfo;

class Player{
public:
	Player(const shared_ptr<Session>& owner);

	void SendData(const vector<shared_ptr<Buffer>>& sendBuffer);

	uint64 GetSessionId();

private:
	shared_ptr<Session> _owner;
};

class PlayerManager {
public:
	void CreatePlayerAndAdd(const shared_ptr<Session>& playerOwner, uint64  userId);
	void RequestRoomList();

	uint32 GetPlayerCount() { return _playerCount; }

private:
	shared_ptr<Player> CreatePlayer(const shared_ptr<Session>& playerOwner);
	void AddPlayer(const shared_ptr<Player>& player, uint64 userId);

private:
	atomic<uint32> _playerCount = 0;
	map<uint64, shared_ptr<Player>> _players;
};

