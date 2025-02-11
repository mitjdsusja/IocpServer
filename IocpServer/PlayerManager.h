#pragma once
class Player{
public:
	Player(int32 userId);
	Player(shared_ptr<Session> owner, int32 userId);
	~Player();

	void SetInfo(wstring name);
	void SetName(wstring name) { _name = name; }

	wstring GetName() { return _name; }
	int32 GetUserId() { return _userId; }

private:
	shared_ptr<Session> _owner;

	int32 _userId = 0;
	wstring _name;

};

class PlayerManager {
public:
	PlayerManager();
	~PlayerManager();

	void CreateAndAddPlayer(shared_ptr<Session> owner, uint64 sessionId, wstring name, int32 userId);
	shared_ptr<Player> GetPlayer(uint64 sessionId);

private:
	mutex _playersMutex;
	map<uint64, shared_ptr<Player>> players;

};

