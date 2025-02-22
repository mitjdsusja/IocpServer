#pragma once

struct PlayerInfo {
	wstring _name;
	int32 _roomId;
};

class Player{
public:
	Player(shared_ptr<Session> owner, wstring name);
	~Player();

	void ClearResource();

	shared_ptr<Session> GetOwner() { return _owner; }

	PlayerInfo GetPlayerInfo();

	void SetInfo(wstring name);
	void SetName(wstring name) { _name = name; }
	void SetRoomId(int32 roomId) { _roomId = roomId; }

	wstring GetName() { return _name; }
	int32 GetRoomId() { return _roomId; }

private:
	shared_ptr<Session> _owner = nullptr;

	wstring _name = L"";
	int32 _roomId = 0;

};

class PlayerManager {
public:
	PlayerManager();
	~PlayerManager();

	void CreateAndAddPlayer(shared_ptr<Session> owner, uint64 sessionId, wstring name);
	shared_ptr<Player> GetPlayer(uint64 sessionId);

	void RemovePlayer(uint64 sessionId);

private:
	mutex _playersMutex;
	map<uint64, shared_ptr<Player>> _players;

};

