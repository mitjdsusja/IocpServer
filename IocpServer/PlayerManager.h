#pragma once
class Player{
public:
	Player();
	Player(shared_ptr<Session> owner);
	~Player();

	void SetInfo(wstring name);
	void SetName(wstring name) { _name = name; }

	wstring GetName() { return _name; }

private:
	shared_ptr<Session> _owner;

	int32 _userId = 0;
	wstring _name;

};

class PlayerManager {
public:
	PlayerManager();
	~PlayerManager();

	void CreateAndAddPlayer(shared_ptr<Session> owner, uint64 sessionId, wstring name);
	shared_ptr<Player> GetPlayer(uint64 sessionId);

private:
	mutex _playersMutex;
	map<uint64, shared_ptr<Player>> players;

};

