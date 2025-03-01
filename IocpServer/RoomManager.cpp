#include "pch.h"
#include "RoomManager.h"
#include "PlayerManager.h"
#include "GameSession.h"

Room::Room(int32 roomId, shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount)
	: _roomId(roomId), _hostPlayer(hostPlayer), _roomName(roomName), _maxPlayerCount(maxPlayerCount){
	AddPlayer(hostPlayer->GetOwner()->GetSessionId(), hostPlayer);
}

Room::~Room() {
	cout << "[REMOVE ROOM] roomId : " << _roomId << endl;
}

void Room::AddPlayer(uint64 sessionId, shared_ptr<Player> player){

	_players[sessionId] = player;
	_curPlayerCount++;
}

void Room::RemovePlayer(uint64 sessionId){

	_players.erase(sessionId);

	if (_players.empty()) {
		GRoomManager->RemoveRoom(_roomId);
	}
}

RoomInfo Room::GetRoomInfo(){

	RoomInfo roomInfo = { _roomId, _maxPlayerCount, _curPlayerCount, _roomName , _hostPlayer->GetName()};
	
	vector<PlayerInfo> playerInfoList;
	for (const auto& player : _players) {
		playerInfoList.push_back(player.second->GetPlayerInfo());
	}
	roomInfo._playerInfoList = playerInfoList;

	return roomInfo;
}

vector<PlayerInfo> Room::GetRoomPlayerInfoList(int32 roomId){

	vector<PlayerInfo> playerInfoList;
	return vector<PlayerInfo>();
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

bool RoomManager::EnterRoom(int32 roomId, int64 sessionId, shared_ptr<Player> player){

	lock_guard<mutex> lock(_roomsMutex);

	shared_ptr<Room> room = _rooms[roomId];
	if (room != nullptr) {
		return false;
	}

	room->AddPlayer(sessionId, player);

	return true;
}

void RoomManager::RemoveRoom(int32 roomId){

	_rooms.erase(roomId);
}

void RoomManager::RemovePlayerFromRoom(int32 roomId, uint64 sessionId){

	shared_ptr<Room> room = _rooms[roomId];
	if (room != nullptr) {
		room->RemovePlayer(sessionId);
	}
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

RoomInfo RoomManager::GetRoomInfo(int32 roomId){

	lock_guard<mutex> lock(_roomsMutex);

	map<int32, shared_ptr<Room>>::iterator iter;
	iter = _rooms.find(roomId);
	if (iter == _rooms.end()) {
		cout << "[INVALID ROOM ID] roomId : " << roomId << endl;
		return RoomInfo();
	}
	
	RoomInfo roomInfo = (*iter).second->GetRoomInfo();
	
	return roomInfo;
}

shared_ptr<Room> RoomManager::MakeRoomPtr(int32 roomId, shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount) {

	return make_shared<Room>(roomId, hostPlayer, roomName, maxPlayerCount);
}

