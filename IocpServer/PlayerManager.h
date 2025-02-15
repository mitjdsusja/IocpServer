#pragma once
class Player{
public:
	Player(shared_ptr<Session> owner, wstring name);
	~Player();

	shared_ptr<Session> GetOwner() { return _owner; }

	void SetInfo(wstring name);
	void SetName(wstring name) { _name = name; }

	wstring GetName() { return _name; }

private:
	shared_ptr<Session> _owner = nullptr;

	wstring _name = L"";

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

