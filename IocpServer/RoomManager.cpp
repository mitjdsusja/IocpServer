#include "pch.h"
#include "RoomManager.h"

Room::Room(int32 roomId, Player* hostPlayer, wstring roomName, int32 maxPlayerCount)
	: _roomId(roomId), _hostPlayer(hostPlayer), _roomName(roomName), _maxPlayerCount(maxPlayerCount){

}

void Room::AddPlayer(int32 playerId, Player* player){

	_players[playerId] = player;
}

void Room::RemovePlayer(int32 playerId){

	_players.erase(playerId);
}

RoomInfo Room::GetRoomInfo(){

	lock_guard<mutex> lock(_roomMutex);

	RoomInfo roomInfo = { _roomId, _maxPlayerCount, _curPlayerCount, _roomName };
	return roomInfo;
}

void RoomManager::CreateAndAddRoom(Player* hostPlayer, wstring roomName, int32 maxPlayerCount){
	
	lock_guard<mutex> lock(_roomsMutex);
		
	// maximum room count
	if (_maxRoomCount <= _rooms.size()) {
		return;
	}

	int32 roomId = _nextRoomId.fetch_add(1);
	shared_ptr<Room> room = make_shared<Room>(roomId, hostPlayer, roomName, maxPlayerCount);
	_rooms[roomId] = room;
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

