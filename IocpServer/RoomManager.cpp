#include "pch.h"
#include "RoomManager.h"

#include "GameSession.h"
#include "messageTest.pb.h"
#include "PacketHandler.h"
#include "Job.h"
#include "TimedJob.h"
#include "JobScheduler.h"
#include "Actor.h"
#include "GridManager.h"
#include "PlayerManager.h"
#include "ActorManager.h"

#include <boost/locale.hpp>


Room::Room(const InitRoomInfo& initRoomInfo, const RoomPlayer& hostPlayerData)
	: _gridManager(make_shared<GridManager>(10)), _roomInfo({initRoomInfo, 0, hostPlayerData._gameState._name, hostPlayerData._sessionId}) , Actor(ActorType::RoomType){

	EnterPlayer(hostPlayerData._sessionId, hostPlayerData);
}

Room::~Room() {

	cout << "[Room::~Room] roomId : " << _roomInfo._initRoomInfo._roomId << endl;
}

void Room::PushJobBroadcast(shared_ptr<Buffer> originSendBuffer){

	shared_ptr<Room> self = static_pointer_cast<Room>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self]() {
		
	});
}

void Room::PushJobBroadcastPosition(){

	shared_ptr<Room> self = static_pointer_cast<Room>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self]() {
		self->BroadcastPlayerMovement();
	});
}

void Room::PushJobRegisterBroadcastPosition(){

	shared_ptr<Room> self = static_pointer_cast<Room>(shared_from_this());

	shared_ptr<ScheduledTimedJob> scheduledTimedJob = make_shared<ScheduledTimedJob>();
	scheduledTimedJob->_timedJobRef = make_unique<TimedJob>(100, [self]() {

		self->BroadcastPlayerMovement();

		if (self->_removeRoomFlag == true) {
			return;
		}

		self->PushJobRegisterBroadcastPosition();
	});
	scheduledTimedJob->_jobQueueRef = self;

	GJobScheduler->RegisterTimedJob(scheduledTimedJob);
}

void Room::PushJobRegisterBroadcastPlayerInGrid(){

	shared_ptr<Room> self = static_pointer_cast<Room>(shared_from_this());

	shared_ptr<ScheduledTimedJob> scheduledTimedJob = make_shared<ScheduledTimedJob>();
	scheduledTimedJob->_timedJobRef = make_unique<TimedJob>(1000, [self]() {

		self->BroadcastPlayerInGrid();

		if (self->_removeRoomFlag == true) {
			return;
		}

		self->PushJobRegisterBroadcastPlayerInGrid();
	});
	scheduledTimedJob->_jobQueueRef = self;

	GJobScheduler->RegisterTimedJob(scheduledTimedJob);
}

void Room::PushJobEnterPlayer(uint64 enterPlayerSessionId, const RoomPlayer& initialPlayerData){

	shared_ptr<Room> self = static_pointer_cast<Room>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, enterPlayerSessionId, initialPlayerData]() {

		bool enterRoomflag = self->EnterPlayer(enterPlayerSessionId, initialPlayerData);

		RoomInfo roomInfo = self->GetRoomInfo();

		PlayerPosition position;
		position._roomId = roomInfo._initRoomInfo._roomId;
		GPlayerManager->PushJobSetPosition(initialPlayerData._sessionId, position);

		// Enter Result Send
		{
			msgTest::SC_Enter_Room_Response enterRoomResponsePacket;
			msgTest::Room* room = enterRoomResponsePacket.mutable_room();

			enterRoomResponsePacket.set_success(true);
			room->set_roomid(roomInfo._initRoomInfo._roomId);
			room->set_roomname(boost::locale::conv::utf_to_utf<char>(roomInfo._initRoomInfo._roomName));
			room->set_maxplayercount(roomInfo._initRoomInfo._maxPlayerCount);
			room->set_playercount(roomInfo._curPlayerCount);
			room->set_hostplayername(boost::locale::conv::utf_to_utf<char>(roomInfo._hostPlayerName));

			vector<shared_ptr<Buffer>> sendBuffer = PacketHandler::MakeSendBuffer(enterRoomResponsePacket, PacketId::PKT_SC_ENTER_ROOM_RESPONSE);

			GPlayerManager->PushJobSendData(enterPlayerSessionId, sendBuffer);
		}

		// Notify All
		{
			msgTest::SC_Player_Enter_Room_Notification enterRoomNotiticationPacket;
			msgTest::Player* player = enterRoomNotiticationPacket.mutable_player();
			msgTest::Vector* position = player->mutable_position();

			player->set_name(boost::locale::conv::utf_to_utf<char>(initialPlayerData._gameState._name));
			player->set_level(0);
			position->set_x(initialPlayerData._gameState._position._x);
			position->set_y(initialPlayerData._gameState._position._y);
			position->set_z(initialPlayerData._gameState._position._z);

			vector<shared_ptr<Buffer>> sendBuffer = PacketHandler::MakeSendBuffer(enterRoomNotiticationPacket, PacketId::PKT_SC_PLAYER_ENTER_ROOM_NOTIFICATION);

			self->Broadcast(sendBuffer);
		}
	});

	PushJob(move(job));
}

void Room::PushJobLeavePlayer(uint64 leavePlayerSessionId){

	shared_ptr<Room> self = static_pointer_cast<Room>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, leavePlayerSessionId]() {
		self->LeavePlayer(leavePlayerSessionId);
	});

	PushJob(move(job));
}

void Room::PushJobMovePlayer(uint64 movePlayerSessionId, const Room::RoomPlayer& roomPlayerData){

	shared_ptr<Room> self = static_pointer_cast<Room>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, movePlayerSessionId, roomPlayerData]() {
		self->MovePlayer(movePlayerSessionId, roomPlayerData);
	});

	PushJob(move(job));
}

void Room::PushJobGetRoomInfo(function<void(RoomInfo& roomInfo)> func){

	shared_ptr<Room> self = static_pointer_cast<Room>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, func]() {

		RoomInfo roomInfo = self->GetRoomInfo();
		func(roomInfo);
	});

	PushJob(move(job));
}

void Room::PushJobGetRoomPlayerList(function<void(vector<Room::RoomPlayer>)> func){

	shared_ptr<Room> self = static_pointer_cast<Room>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, func]() {

		vector<RoomPlayer> roomPlayerList = self->GetRoomPlayerList();
		func(roomPlayerList);
	});

	PushJob(move(job));
}

int32 Room::GetPlayerCount(){

	return _players.size();
}


void Room::RegisterBroadcastMovement(uint32 reserveTime){

	BroadcastPlayerMovement();
	//cout << "[Room " << _roomId << "] Broadcast Movement " << endl;

	if (_removeRoomFlag == true) return;

	/*GJobTimer->Reserve(reserveTime, [thisRoomRef = dynamic_pointer_cast<Room>(shared_from_this()), reserveTime]() {
		thisRoomRef->RegisterBroadcastMovement(reserveTime);
	});*/
}

void Room::DestroyRoom(){

	_removeRoomFlag = true;
	GRoomManager->PushJobRemoveRoom(_roomInfo._initRoomInfo._roomId);
}

void Room::Broadcast(const shared_ptr<Buffer>& sendBuffer){

	for (const auto& p : _players) {

		uint64 sessionId = p.first;

		GPlayerManager->PushJobSendData(sessionId, sendBuffer);
	}
}

void Room::Broadcast(const vector<shared_ptr<Buffer>>& sendBuffer){

	for (const auto& p : _players) {

		uint64 sessionId = p.first;

		GPlayerManager->PushJobSendData(sessionId, sendBuffer);
	}
}

void Room::BroadcastPlayerMovement() {

	unordered_map<uint64, msgTest::MoveState> updatedMoveStates;

	for (auto& p : _players) {

		auto& playerData = p.second;

		if (playerData._gameState._updatePosition == false) {
			continue;
		}
		playerData._gameState._updatePosition = false;

		msgTest::MoveState moveState;
		msgTest::Vector* position = moveState.mutable_position();
		msgTest::Vector* velocity = moveState.mutable_velocity();
		msgTest::Vector* rotation = moveState.mutable_rotation();

		moveState.set_playername(boost::locale::conv::utf_to_utf<char>(playerData._gameState._name));
		position->set_x(playerData._gameState._position._x);
		position->set_y(playerData._gameState._position._y);
		position->set_z(playerData._gameState._position._z);
		velocity->set_x(playerData._gameState._velocity._x);
		velocity->set_y(playerData._gameState._velocity._y);
		velocity->set_z(playerData._gameState._velocity._z);
		rotation->set_x(playerData._gameState._rotation._x);
		rotation->set_y(playerData._gameState._rotation._y);
		rotation->set_z(playerData._gameState._rotation._z);
		
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

		GPlayerManager->PushJobSendData(player._sessionId, sendBuffer);
	}
}

void Room::BroadcastPlayerInGrid(){

	for (auto& p : _players) {

		uint64 sessionId = p.first;
		RoomPlayer& curPlayer = p.second;
		vector<uint64> sessionIdInGrid;

		sessionIdInGrid = _gridManager->GetNearByPlayers(sessionId);

		msgTest::SC_Player_List_In_Grid sendPlayerListInGridPacket;

		for (auto& sessionId : sessionIdInGrid) {
			string* name = sendPlayerListInGridPacket.add_playernamelist();
			*name = boost::locale::conv::utf_to_utf<char>(_players[sessionId]._gameState._name);
		}

		vector<shared_ptr<Buffer>> sendBuffer = PacketHandler::MakeSendBuffer(sendPlayerListInGridPacket, PacketId::PKT_SC_PLAYER_LIST_IN_GRID);

		GPlayerManager->PushJobSendData(sessionId, sendBuffer);
	}
}

bool Room::EnterPlayer(uint64 sessionId, const RoomPlayer& playerData) {

	_players[sessionId] = playerData;
	_roomInfo._curPlayerCount++;

	_gridManager->AddPlayer(sessionId, playerData._gameState._position);

	spdlog::info("[Room::EnterPlayer] roomId : {}", _roomInfo._initRoomInfo._roomId);
	//wcout << L"[Room::EnterPlayer] roomId : " << _roomInfo._initRoomInfo._roomId << " EnterPlayer : " << playerData._gameState._name << endl;

	return true;
}

void Room::LeavePlayer(uint64 sessionId) {

	_players.erase(sessionId);
	_gridManager->RemovePlayer(sessionId);

	spdlog::info("[Room::LeavePlayer] RoomId : {}", _roomInfo._initRoomInfo._roomId);
	//wcout << L"[Room::LeavePlayer] RoomId : " << _roomInfo._initRoomInfo._roomId << " LeavePlayer : " << sessionId << " Remained Count : " << _players.size() << endl;

	if (_players.size() == 0) {
		DestroyRoom();
	}
}

void Room::MovePlayer(uint64 sessionId, const Room::RoomPlayer& roomPlayerData) {

	_players[sessionId]._gameState = roomPlayerData._gameState;

	_gridManager->MovePosition(sessionId, roomPlayerData._gameState._position);
}

RoomInfo Room::GetRoomInfo() {

	return _roomInfo;
}

vector<Room::RoomPlayer> Room::GetRoomPlayerList() {

	vector<Room::RoomPlayer> roomPlayerList;
	for (const auto& p : _players) {

		const RoomPlayer& roomPlayer = p.second;
		roomPlayerList.push_back(roomPlayer);
	}

	return roomPlayerList;
}

/*-----------------
	RoomManager
-------------------*/

RoomManager::RoomManager(int32 maxRoomCount) : _maxRoomCount(maxRoomCount), Actor(ActorType::RoomManagerType){

}

void RoomManager::PushJobCreateAndPushRoom(const InitRoomInfo& initRoomInfo, const Room::RoomPlayer& hostPlayerData){

	shared_ptr<RoomManager> self = static_pointer_cast<RoomManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, initRoomInfo, hostPlayerData]() {
		int32 roomId = self->CreateAndPushRoom(initRoomInfo, hostPlayerData);

		if (roomId != 0) {
			PlayerPosition position;
			position._roomId = roomId;
			GPlayerManager->PushJobSetPosition(hostPlayerData._sessionId, position);
		}

		msgTest::SC_Create_Room_Response createRoomResponsePacket;
		msgTest::Room* room = createRoomResponsePacket.mutable_room();

		if (roomId != 0) {
			createRoomResponsePacket.set_success(true);
			createRoomResponsePacket.set_errormessage("Create Room ");
			room->set_roomid(roomId);
			room->set_roomname(boost::locale::conv::utf_to_utf<char>(initRoomInfo._roomName));
			room->set_playercount(1);
			room->set_maxplayercount(initRoomInfo._maxPlayerCount);
			room->set_hostplayername(boost::locale::conv::utf_to_utf<char>(hostPlayerData._gameState._name));
		}
		else {
			createRoomResponsePacket.set_success(false);
			createRoomResponsePacket.set_errormessage("FAIL ENTER ROOM");
		}

		vector<shared_ptr<Buffer>> sendBuffer = PacketHandler::MakeSendBuffer(createRoomResponsePacket, PacketId::PKT_SC_CREATE_ROOM_RESPONSE);

		GPlayerManager->PushJobSendData(hostPlayerData._sessionId, sendBuffer);
	});

	PushJob(move(job));
}

void RoomManager::PushJobEnterRoom(int32 roomId, const Room::RoomPlayer& enterPlayerData){

	shared_ptr<RoomManager> self = static_pointer_cast<RoomManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, roomId, enterPlayerData]() {
		self->EnterRoom(roomId, enterPlayerData);
	});

	PushJob(move(job));
}

void RoomManager::PushJobLeaveRoom(int32 roomId, uint64 sessionId){

	shared_ptr<RoomManager> self = static_pointer_cast<RoomManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, roomId, sessionId]() {
		self->LeaveRoom(roomId, sessionId);
	});

	PushJob(move(job));
}

void RoomManager::PushJobRemoveRoom(int32 roomId){

	shared_ptr<RoomManager> self = static_pointer_cast<RoomManager>(shared_from_this());
	
	unique_ptr<Job> job = make_unique<Job>([self, roomId]() {
		self->RemoveRoom(roomId);
	});

	PushJob(move(job));
}

void RoomManager::PushJobMovePlayer(int32 roomId, const Room::RoomPlayer& roomPlayerData){

	shared_ptr<RoomManager> self = static_pointer_cast<RoomManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, roomId, roomPlayerData]() {

		self->MovePlayer(roomId, roomPlayerData);
	});

	PushJob(move(job));
}

void RoomManager::PushJobGetRoomInfoList(function<void(vector<RoomInfo>)> func){

	shared_ptr<RoomManager> self = static_pointer_cast<RoomManager>(shared_from_this());
	
	unique_ptr<Job> job = make_unique<Job>([self, func]() {
		
		shared_ptr<atomic<int32>> pendingRoomsCountRef = make_shared<atomic<int32>>((int32)self->_rooms.size());
		shared_ptr<vector<RoomInfo>> roomInfoListRef = make_shared<vector<RoomInfo>>();
		shared_ptr<mutex> roomInfoListMutexRef = make_shared<mutex>();

		for (const auto& p : self->_rooms) {

			shared_ptr<Room> room = p.second;

			room->PushJobGetRoomInfo([func, pendingRoomsCountRef, roomInfoListRef, roomInfoListMutexRef](const RoomInfo& roomInfo) {
				{
					lock_guard<mutex> lock(*roomInfoListMutexRef);
					roomInfoListRef->push_back(roomInfo);
				}
				if (pendingRoomsCountRef->fetch_sub(1) == 1) {
					unique_ptr<Job> job = make_unique<Job>([func, roomInfoListRef]() {
						func(*roomInfoListRef);
					});
					GRoomManager->PushJob(move(job));
				}
			});
		}
	});

	this->PushJob(move(job));
}

void RoomManager::PushJobGetRoomPlayerList(int32 roomId, function<void(vector<Room::RoomPlayer>)> func){

	shared_ptr<RoomManager> self = static_pointer_cast<RoomManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, roomId, func]() {

		const auto& p = self->_rooms.find(roomId);
		if (p == self->_rooms.end()) {

			spdlog::info("[RoomManager::PushJobGetRoomPlayerList] Invalid RoomId : {}", roomId);
			//cout << "[RoomManager::PushJobGetRoomPlayerList] Invalid RoomId : " << roomId << endl;
			return;
		}
		const shared_ptr<Room>& room = p->second;
		room->PushJobGetRoomPlayerList(func);
	});

	this->PushJob(move(job));
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

int32 RoomManager::CreateAndPushRoom(const InitRoomInfo& initRoomInfo, const Room::RoomPlayer& hostPlayerData){
	
	// maximum room check
	if (_maxRoomCount <= _rooms.size()) {
		return 0;
	}

	int32 roomId = _nextRoomId.fetch_add(1);
	InitRoomInfo roomInfo = initRoomInfo;
	roomInfo._roomId = roomId;

	shared_ptr<Room> room = MakeRoomPtr(roomInfo, hostPlayerData);
	room->SetActorId(GActorManager->RegisterActor(room));
	_rooms[roomId] = room;

	spdlog::info("[RoomManager::CreateAndPushRoom] EnterRoom roomId : {} playerName : {}", roomId, boost::locale::conv::utf_to_utf<char>(hostPlayerData._gameState._name));
	//wcout << "[RoomManager::CreateAndPushRoom] EnterRoom roomId : " << roomId << " playerName : " << hostPlayerData._gameState._name << endl;

	room->PushJobRegisterBroadcastPosition();
	room->PushJobRegisterBroadcastPlayerInGrid();

	return roomId;
}

bool RoomManager::EnterRoom(int32 roomId, const Room::RoomPlayer& enterPlayerData){

	shared_ptr<Room>& room = _rooms[roomId];
	if (room == nullptr) {
		return false;
	}

	room->PushJobEnterPlayer(enterPlayerData._sessionId, enterPlayerData);

	return true;
}

void RoomManager::LeaveRoom(int32 roomId, uint64 sessionId) {

	auto it = _rooms.find(roomId);
	if (it == _rooms.end()) {

		spdlog::info("[RoomManager::LeaveRoom] Invalid Room : {}", roomId);
		//cout << "[RoomManager::LeaveRoom] Invalid Room : " << roomId << endl;
		return;
	}

	shared_ptr<Room> room = it->second;
	room->PushJobLeavePlayer(sessionId);
}

void RoomManager::RemoveRoom(int32 roomId) {

	auto it = _rooms.find(roomId);
	if (it != _rooms.end()) {

		_rooms.erase(it);
		spdlog::info("[RoomManager::RemoveRoom] RemoveRoom : {}", roomId);
		//cout << "[RoomManager::RemoveRoom] RemoveRoom : " << roomId << endl;
	}
	else {
		spdlog::info("[RoomManager::RemoveRoom] Invalid Room : {}", roomId);
		//cout << "[RoomManager::RemoveRoom] Invalid Room : " << roomId << endl;
	}
}

void RoomManager::MovePlayer(int32 roomId, const Room::RoomPlayer& roomPlayerData){

	const auto& iter = _rooms.find(roomId);
	if (iter == _rooms.end()) {

		spdlog::info("[RoomManager::MovePlayer] Invalid Room : {}", roomId);
		//cout << "[RoomManager::MovePlayer] Invalid Room : " << roomId << endl;
		return;
	}
	shared_ptr<Room>& room = iter->second;

	room->PushJobMovePlayer(roomPlayerData._sessionId, roomPlayerData);
}

shared_ptr<Room> RoomManager::MakeRoomPtr(const InitRoomInfo& initRoomInfo, const Room::RoomPlayer& hostPlayerData) {

	return make_shared<Room>(initRoomInfo, hostPlayerData);
}

