#include "pch.h"
#include "RoomManager.h"
#include "GameSession.h"
#include "jobQueue.h"
#include "messageTest.pb.h"
#include "PacketHandler.h"
#include "Job.h"
#include "JobTimer.h"
#include "JobScheduler.h"

#include <boost/locale.hpp>

Room::Room(int32 roomId, wstring roomName, int32 maxPlayerCount, RoomPlayer hostPlayerData)
	: _gridManager(make_shared<GridManager>(10)), _roomId(roomId), _hostPlayerSessionId(hostPlayerData._sessionId), _roomName(roomName), _maxPlayerCount(maxPlayerCount){

	AddPlayer(hostPlayerData._sessionId, hostPlayerData);
}

Room::~Room() {

	cout << "[REMOVE ROOM] roomId : " << _roomId << endl;
}

void Room::Broadcast(shared_ptr<Buffer> originSendBuffer){

}

void Room::AddPlayer(uint64 sessionId, RoomPlayer playerData){

	_players[sessionId] = playerData;
	_curPlayerCount++;

	_gridManager->AddPlayer(sessionId, playerData._gameState._position);
}

void Room::RemovePlayer(uint64 sessionId){

	_players.erase(sessionId);
	_gridManager->RemovePlayer(sessionId);

	if (_players.size() == 0) {
		_hostPlayerSessionId = 0;
		DestroyRoom();
	}
}

void Room::MovePlayer(uint64 sessionId, Vector<int16> newPosition){

	_gridManager->MovePosition(sessionId, newPosition);
}

void Room::BroadcastPlayerMovement(){

	unordered_map<uint64, msgTest::MoveState> updatedMoveStates;

	for (auto& p : _players) {

		auto& playerData = p.second;

		if (playerData._gameState._updatePosition == false) {
			continue;
		}

		msgTest::MoveState moveState;
		msgTest::Vector* position = moveState.mutable_position();
		msgTest::Vector* velocity = moveState.mutable_velocity();

		moveState.set_playername(boost::locale::conv::utf_to_utf<char>(playerData._gameState._name));
		position->set_x(playerData._gameState._position._x);
		position->set_y(playerData._gameState._position._y);
		position->set_z(playerData._gameState._position._z);
		velocity->set_x(playerData._gameState._velocity._x);
		velocity->set_y(playerData._gameState._velocity._y);
		velocity->set_z(playerData._gameState._velocity._z);
		moveState.set_timestamp(playerData._gameState._moveTimeStamp);

		updatedMoveStates[p.first] = move(moveState);
	}
	
	for (auto& p : _players) {
		
		auto& player = p.second;

		vector<uint64> nearPlayerSessionIdList = _gridManager->GetNearByPlayers(p.first);

		msgTest::SC_Player_Move_Notification sendPlayerMoveNotificationPacket;

		for (uint64 targetSessionId : nearPlayerSessionIdList) {
			
			auto it = updatedMoveStates.find(targetSessionId);
			if (it != updatedMoveStates.end()) {
				msgTest::MoveState* moveState = sendPlayerMoveNotificationPacket.add_movestates();
				*moveState = it->second;
			}
		}

		if (sendPlayerMoveNotificationPacket.movestates_size() == 0) {
			continue;
		}

		auto sendBuffer = PacketHandler::MakeSendBuffer(sendPlayerMoveNotificationPacket, PacketId::PKT_SC_PLAYER_MOVE_NOTIFICATION);

		GPlayerManager->PushSendJob(player._sessionId, sendBuffer);
	}
}

RoomInfo Room::GetRoomInfo(){

	RoomInfo roomInfo = { _roomId, _maxPlayerCount, _curPlayerCount, _roomName , _players[_hostPlayerSessionId]._gameState._name};
	
	vector<uint64> playersSessionId;
	for (const auto& player : _players) {
		playersSessionId.push_back(player.second._sessionId);
	}
	roomInfo._playersSessionId = playersSessionId;

	return roomInfo;
}

int32 Room::GetPlayerCount(){

	return _players.size();
}

void Room::RegisterBroadcastMovement(uint32 reserveTime){

	BroadcastPlayerMovement();
	//cout << "[Room " << _roomId << "] Broadcast Movement " << endl;

	if (_removeRoomFlag == true) return;

	GJobTimer->Reserve(reserveTime, [thisRoomRef = dynamic_pointer_cast<Room>(shared_from_this()), reserveTime]() {
		thisRoomRef->RegisterBroadcastMovement(reserveTime);
	});
}

void Room::DestroyRoom(){

	_removeRoomFlag = true;
}

/*-----------------
	RoomManager
-------------------*/

RoomManager::RoomManager(int32 maxRoomCount) : _maxRoomCount(maxRoomCount){

}

void RoomManager::BroadcastToRoom(int32 roomId, shared_ptr<Buffer> sendBuffer){

	shared_ptr<Room> room;
	{
		lock_guard<mutex> lock(_roomsMutex);

		room = _rooms[roomId];
		if (room == nullptr) {
			//cout << "[INVALID ROOM] roomId : " << roomId << endl;
			return;
		}
	}
	room->Broadcast(sendBuffer);
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

	hostPlayer->SetJoinedRoom(room);

	room->RegisterBroadcastMovement(100);

	return roomId;
}

bool RoomManager::EnterRoom(int32 roomId, int64 sessionId, Room::RoomPlayer enterPlayerData){

	lock_guard<mutex> lock(_roomsMutex);

	shared_ptr<Room> room = _rooms[roomId];
	if (room == nullptr) {
		return false;
	}

	room->AddPlayer(sessionId, enterPlayerData);
	//wcout << "Enter Room : " << player->GetName() << " Total Player : " << room->GetPlayerCount() << endl;

	return true;
}

void RoomManager::RemoveRoom(int32 roomId) {

	lock_guard<mutex> lock(_roomsMutex); 

	auto it = _rooms.find(roomId);
	if (it != _rooms.end()) {
		_rooms.erase(it);
	}
}

void RoomManager::LeavePlayerFromRoom(int32 roomId, uint64 sessionId) {

	lock_guard<mutex> lock(_roomsMutex); 

	auto it = _rooms.find(roomId);
	if (it == _rooms.end()) {
		//cout << "[INVALID ROOM ID] room ID : " << roomId << endl;
		return; 
	}

	shared_ptr<Room> room = it->second;
	room->RemovePlayer(sessionId);

	const auto& player = GPlayerManager->GetPlayer(sessionId);
	wcout << "[Leave Player] " << player->GetName() << " From " << room->GetRoomInfo()._roomId << endl;

	if (room->GetPlayerCount() == 0) {
		_rooms.erase(it); 
	}
	//cout << "Reamined Room Count : " << _rooms.size() << endl;
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
		//cout << "[INVALID ROOM ID] roomId : " << roomId << endl;
		return RoomInfo();
	}
	
	RoomInfo roomInfo = (*iter).second->GetRoomInfo();
	
	return roomInfo;
}

shared_ptr<Room> RoomManager::MakeRoomPtr(int32 roomId, shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount) {

	return make_shared<Room>(roomId, hostPlayer, roomName, maxPlayerCount);
}

