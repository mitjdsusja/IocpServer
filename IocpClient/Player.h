#pragma once

class Player{
public:
	Player(shared_ptr<Session> owner);

	uint64 GetSessionId();

private:
	shared_ptr<Session> _owner;
};

class PlayerManager {
public:
	void CreatePlayerAndAdd(const shared_ptr<Session>& playerOwner);
	void LoginRequest();

private:
	shared_ptr<Player> CreatePlayer(const shared_ptr<Session>& playerOwner);
	void AddPlayer(shared_ptr<Player>& player);

private:
	map<uint64, shared_ptr<Player>> _players;
};

