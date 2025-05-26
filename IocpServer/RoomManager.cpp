#include "pch.h"
#include "RoomManager.h"
#include "GameSession.h"
#include "jobQueue.h"
#include "messageTest.pb.h"
#include "PacketHandler.h"
#include "JobTimer.h"

#include <boost/locale.hpp>

Room::Room(int32 roomId, shared_ptr<Player> hostPlayer, wstring roomName, int32 maxPlayerCount)
	: _gridManager(make_shared<GridManager>(10)), _roomId(roomId), _hostPlayer(hostPlayer), _roomName(roomName), _maxPlayerCount(maxPlayerCount){

	AddPlayer(hostPlayer->GetOwner()->GetSessionId(), hostPlayer);
}

Room::~Room() {

	cout << "[REMOVE ROOM] roomId : " << _roomId << endl;
}

void Room::Broadcast(shared_ptr<Buffer> originSendBuffer){

	lock_guard<mutex> lock(_roomMutex);

	for (auto& it : _players) {
		shared_ptr<GameSession> gameSession = it.second->GetOwner();

		shared_ptr<Buffer> sendBuffer = shared_ptr<Buffer>(GSendBufferPool->Pop(), [](Buffer* buffer) { GSendBufferPool->Push(buffer); });
		memcpy(sendBuffer->GetBuffer(), originSendBuffer->GetBuffer(), originSendBuffer->WriteSize());
		sendBuffer->Write(originSendBuffer->WriteSize());

		//wcout << "Broadcast : " << it.second->GetName() << endl;
		Job* job = new Job([gameSession, sendBuffer]() {
			gameSession->Send(sendBuffer);
		});
		GJobQueue->Push(job);
	}
}

void Room::AddPlayer(uint64 sessionId, shared_ptr<Player> player){

	lock_guard<mutex> lock(_roomMutex);

	_players[sessionId] = player;
	_curPlayerCount++;

	_gridManager->AddPlayer(sessionId, player->GetPlayerInfo()._position);
}

void Room::RemovePlayer(uint64 sessionId){

	lock_guard<mutex> lock(_roomMutex);

	_players.erase(sessionId);
	_gridManager->RemovePlayer(sessionId);

	if (_players.size() == 0) {
		_hostPlayer = nullptr;
		DestroyRoom();
	}
}

void Room::MovePlayer(uint64 sessionId, Vector<int16> newPosition){

	lock_guard<mutex> _lock(_roomMutex);

	_gridManager->MovePosition(sessionId, newPosition);
}

void Room::BroadcastPlayerMovement(){

	lock_guard<mutex> _lock(_roomMutex);

	unordered_map<uint64, msgTest::MoveState> updatedMoveStates;

	for (auto& p : _players) {

		auto& player = p.second;

		PlayerInfo playerInfo = player->GetPlayerInfo();
		if (playerInfo._isInfoUpdated == false) {
			continue;
		}

		msgTest::MoveState moveState;
		msgTest::Vector* position = moveState.mutable_position();
		msgTest::Vector* velocity = moveState.mutable_velocity();

		moveState.set_playername(boost::locale::conv::utf_to_utf<char>(playerInfo._name));
		position->set_x(playerInfo._position._x);
		position->set_y(playerInfo._position._y);
		position->set_z(playerInfo._position._z);
		velocity->set_x(playerInfo._velocity._x);
		velocity->set_y(playerInfo._velocity._y);
		velocity->set_z(playerInfo._velocity._z);
		moveState.set_timestamp(playerInfo._moveTimestamp);

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

		auto targetSession = player->GetOwner();
		Job* job = new Job([session = targetSession, sendBuffer]() {
			session->Send(sendBuffer);
		});

		GJobQueue->Push(job);
	}
}

RoomInfo Room::GetRoomInfo(){

	RoomInfo roomInfo = { _roomId, _maxPlayerCount, _curPlayerCount, _roomName , _hostPlayer->GetName()};
	
	lock_guard<mutex> lock(_roomMutex);

	vector<PlayerInfo> playerInfoList;
	for (const auto& player : _players) {
		playerInfoList.push_back(player.second->GetPlayerInfo());
	}
	roomInfo._playerInfoList = playerInfoList;

	return roomInfo;
}

int32 Room::GetPlayerCount(){

	return _players.size();
}

void Room::RegisterBroadcastMovement(uint32 reserveTime){

	BroadcastPlayerMovement();
	//cout << "[Room " << _roomId << "] Broadcast Movement " << endl;

	if (_removeRoomFlag == true) return;

	GJobTimer->Reserve(reserveTime, [thisRoomRef = shared_from_this(), reserveTime]() {
		thisRoomRef->RegisterBroadcastMovement(reserveTime);
	});
}

void Room::DestroyRoom(){

	_removeRoomFlag = true;
}

vector<PlayerInfo> Room::GetRoomPlayerInfoList(int32 roomId){

	vector<PlayerInfo> playerInfoList;

	lock_guard<mutex> _lock(_roomMutex);
	
	for (auto& p : _players) {
		auto& player = p.second;

		playerInfoList.push_back(player->GetPlayerInfo());
	}

	return playerInfoList;
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

bool RoomManager::EnterRoom(int32 roomId, int64 sessionId, shared_ptr<Player> player){

	lock_guard<mutex> lock(_roomsMutex);

	shared_ptr<Room> room = _rooms[roomId];
	if (room == nullptr) {
		return false;
	}

	room->AddPlayer(sessionId, player);
	player->SetJoinedRoom(room);
	wcout << "Enter Room : " << player->GetName() << " Total Player : " << room->GetPlayerCount() << endl;

	return true;
}

void RoomManager::RemoveRoom(int32 roomId) {

	lock_guard<mutex> lock(_roomsMutex); 

	auto it = _rooms.find(roomId);
	if (it != _rooms.end()) {
		_rooms.erase(it);
	}
}

void RoomManager::RemovePlayerFromRoom(int32 roomId, uint64 sessionId) {

	lock_guard<mutex> lock(_roomsMutex); 

	auto it = _rooms.find(roomId);
	if (it == _rooms.end()) {
		//cout << "[INVALID ROOM ID] room ID : " << roomId << endl;
		return; 
	}

	shared_ptr<Room> room = it->second;
	room->RemovePlayer(sessionId);

	const auto& player = GPlayerManager->GetPlayer(sessionId);
	wcout << "Remove Player From Room : " << player->GetName() << endl;

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

