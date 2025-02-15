#pragma once

class Player;

struct RoomInfo {
	int32 _roomId;
	int32 _maxPlayerCount;
	int32 _curPlayerCount;
	wstring _roomName;
	wstring _hostPlayerName;
};

class Room {
public:
	Room(int32 roomId, shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount);

	void AddPlayer(uint64 sessionId, shared_ptr<Player> player);
	void RemovePlayer(uint64 sessionId);
	RoomInfo GetRoomInfo();

private:
	mutex _roomMutex;

	int32 _roomId = 0;
	int32 _maxPlayerCount = 0;
	int32 _curPlayerCount = 0;
	wstring _roomName = L"NULL";
	shared_ptr<Player> _hostPlayer = nullptr;
	map<uint64, shared_ptr<Player>> _players;

};

class RoomManager{
public:
	RoomManager(int32 maxRoomCount = 100);
	int32 CreateAndAddRoom(shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount = 10);
	
	vector<RoomInfo> GetRoomInfoList();

private:
	static shared_ptr<Room> MakeRoomPtr(int32 roomId, shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount);

private:
	mutex _roomsMutex;

	int32 _maxRoomCount;
	atomic<int32> _nextRoomId = 1;
	map<int32, shared_ptr<Room>> _rooms;

};

