#pragma once

class Player;

struct RoomInfo {
	int32 _roomId;
	int32 _maxPlayerCount;
	int32 _curPlayerCount;
	wstring _roomName;
};

class Room {
public:
	Room(int32 roomId, shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount);

	void AddPlayer(int32 playerId, shared_ptr<Player> player);
	void RemovePlayer(int32 playerId);
	RoomInfo GetRoomInfo();

private:
	mutex _roomMutex;

	int32 _roomId;
	int32 _maxPlayerCount;
	int32 _curPlayerCount;
	wstring _roomName;
	shared_ptr<Player> _hostPlayer;
	map<int32, shared_ptr<Player>> _players;

};

class RoomManager{
public:
	void CreateAndAddRoom(shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount = 10);
	
	vector<RoomInfo> GetRoomInfoList();

private:
	mutex _roomsMutex;

	int32 _maxRoomCount;
	atomic<int32> _nextRoomId = 1;
	map<int32, shared_ptr<Room>> _rooms;

};

