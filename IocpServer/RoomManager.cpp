#include "pch.h"
#include "RoomManager.h"
#include "PlayerManager.h"

Room::Room(int32 roomId, shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount)
	: _roomId(roomId), _hostPlayer(hostPlayer), _roomName(roomName), _maxPlayerCount(maxPlayerCount){
	AddPlayer(hostPlayer->GetUserId(), hostPlayer);
}

void Room::AddPlayer(int32 playerId, shared_ptr<Player> player){

	_players[playerId] = player;
	_curPlayerCount++;
}

void Room::RemovePlayer(int32 playerId){

	_players.erase(playerId);
}

RoomInfo Room::GetRoomInfo(){

	lock_guard<mutex> lock(_roomMutex);

	RoomInfo roomInfo = { _roomId, _maxPlayerCount, _curPlayerCount, _roomName , _hostPlayer->GetName()};
	return roomInfo;
}

RoomManager::RoomManager(int32 maxRoomCount) : _maxRoomCount(maxRoomCount){

}

int32 RoomManager::CreateAndAddRoom(shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount){
	
	lock_guard<mutex> lock(_roomsMutex);
		
	// maximum room count
	if (_maxRoomCount <= _rooms.size()) {
		return 0;
	}

	int32 roomId = _nextRoomId.fetch_add(1);
	shared_ptr<Room> room = MakeRoomPtr(roomId, hostPlayer, roomName, maxPlayerCount);
	_rooms[roomId] = room;

	return roomId;
}

vector<RoomInfo> RoomManager::GetRoomInfoList(){

	vector<RoomInfo> roomInfoList;
	{
		lock_guard<mutex> lock(_roomsMutex);

		for (const auto& p : _rooms) {
			RoomInfo roomInfo = (p.second)->GetRoomInfo();
			roomInfoList.push_back(roomInfo);
		}
	}
	
	return roomInfoList;
}

shared_ptr<Room> RoomManager::MakeRoomPtr(int32 roomId, shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount) {

	return make_shared<Room>(roomId, hostPlayer, roomName, maxPlayerCount);
}

