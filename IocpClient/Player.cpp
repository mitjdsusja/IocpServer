#include "pch.h"
#include "Player.h"
#include "Global.h"

#include "GameSession.h"
#include "PacketHandler.h"
#include "GameManager.h"

#include "messageTest.pb.h"

const float MAP_MIN_X = -20.0f;
const float MAP_MAX_X = 20.0f;
const float MAP_MIN_Z = -20.0f;
const float MAP_MAX_Z = 20.0f;

Player::Player(const shared_ptr<Session>& owner) : _owner(owner) {

	_playerInfo._position = { 0, 1, 0 };
	_playerInfo._velocity = { 0, 0, 0 };
}

void Player::RandomMove() {
	using namespace std::chrono;

	static thread_local std::mt19937 rng(std::random_device{}());
	static thread_local std::uniform_real_distribution<float> dirDist(-1.0f, 1.0f);
	static thread_local std::uniform_real_distribution<float> timeDist(1.0f, 3.0f); // 방향 유지 시간

	// 방향과 남은 유지 시간을 멤버로 둔다
	if (_playerInfo._moveTimeRemaining <= 0.0f) {
		float dirX = dirDist(rng);
		float dirZ = dirDist(rng);

		float length = std::sqrt(dirX * dirX + dirZ * dirZ);
		if (length != 0.0f) {
			dirX /= length;
			dirZ /= length;
		}

		_playerInfo._velocity._x = dirX;
		_playerInfo._velocity._z = dirZ;
		_playerInfo._moveTimeRemaining = timeDist(rng);
	}

	float deltaTime = 0.1f; // 프레임 간격
	_playerInfo._moveTimeRemaining -= deltaTime;

	float speed = static_cast<float>(_playerInfo._moveSpeed);
	float dx = _playerInfo._velocity._x * speed * deltaTime;
	float dz = _playerInfo._velocity._z * speed * deltaTime;

	float newX = _playerInfo._position._x + dx;
	float newZ = _playerInfo._position._z + dz;

	// 경계 체크 후 반전 + 위치 클램프
	if (newX < MAP_MIN_X) {
		newX = MAP_MIN_X;
		_playerInfo._velocity._x = -_playerInfo._velocity._x;
	}
	else if (newX > MAP_MAX_X) {
		newX = MAP_MAX_X;
		_playerInfo._velocity._x = -_playerInfo._velocity._x;
	}

	if (newZ < MAP_MIN_Z) {
		newZ = MAP_MIN_Z;
		_playerInfo._velocity._z = -_playerInfo._velocity._z;
	}
	else if (newZ > MAP_MAX_Z) {
		newZ = MAP_MAX_Z;
		_playerInfo._velocity._z = -_playerInfo._velocity._z;
	}

	_playerInfo._position._x = newX;
	_playerInfo._position._z = newZ;
}

void Player::SendData(const vector<shared_ptr<Buffer>>& sendBuffer) {

	for (auto& buffer : sendBuffer) {

		_owner->Send(buffer);
	}
}

uint64 Player::GetSessionId() {

	return _owner->GetSessionId();
}

PlayerData Player::GetPlayerInfo(){

	return _playerInfo;
}

void Player::SetName(string name) {

	_playerInfo._name = name;
}

void Player::SetEnterRoomId(int32 roomId) {

	_playerInfo._enterRoomId = roomId;
}

void PlayerManager::CreatePlayerAndAdd(const shared_ptr<Session>& playerOwner, uint64 userId){

	shared_ptr<Player> player = CreatePlayer(playerOwner);
	player->SetName("bot" + to_string(userId));

	AddPlayer(player, userId);
}

void PlayerManager::RequestRoomList(){

	msgTest::CS_Room_List_Request sendPacketRoomListRequest;

	if (_players.size() == 0) {

		spdlog::info("Empty Players ");
		return;
	}

	const auto& iter = _players.begin();
	auto& playerRef = iter->second;

	vector<shared_ptr<Buffer>> buffer = PacketHandler::MakeSendBuffer(sendPacketRoomListRequest, PacketId::PKT_CS_ROOM_LIST_REQUEST);
	//spdlog::info("Buffer : {}", buffer.size());
	playerRef->SendData(buffer);
}

void PlayerManager::RequestEnterRoom(uint64 sessionId, uint32 roomId) {

	msgTest::CS_Enter_Room_Request sendPacketEnterRoomRequest;
	sendPacketEnterRoomRequest.set_roomid(roomId);

	const auto& sendBuffer = PacketHandler::MakeSendBuffer(sendPacketEnterRoomRequest, PacketId::PKT_CS_ENTER_ROOM_REQUEST);

	_players[sessionId]->SendData(sendBuffer);
}

void PlayerManager::RequestCreateRoom(int32 createRoomCount) {

	msgTest::CS_Create_Room_Request sendPacketCreateRoomReqeust;
	
	int32 createCount = 0;
	for (auto& p : _players) {

		if (createCount == createRoomCount) break;

		auto& player = p.second;
		const PlayerData& playerInfo = player->GetPlayerInfo();

		sendPacketCreateRoomReqeust.set_hostname(playerInfo._name);
		sendPacketCreateRoomReqeust.set_roomname("Room" + playerInfo._name);

		const auto& sendBuffer = PacketHandler::MakeSendBuffer(sendPacketCreateRoomReqeust, PacketId::PKT_CS_CREATE_ROOM_REQUEST);

		player->SendData(sendBuffer);
		
		++createCount;
	}
}

void PlayerManager::AllPlayerRequestEnterRoom(uint32 roomId){

	msgTest::CS_Enter_Room_Request sendPacketEnterRoomReqeust;
	sendPacketEnterRoomReqeust.set_roomid(roomId);

	vector<shared_ptr<Buffer>> sendBuffers = PacketHandler::MakeSendBuffer(sendPacketEnterRoomReqeust, PacketId::PKT_CS_ENTER_ROOM_REQUEST);

	for (auto& p : _players) {
		auto& player = p.second;

		player->SendData(sendBuffers);
	}
}

void PlayerManager::AllPlayerRandomMove(){

	for (auto& p : _players) {

		auto& player = p.second;

		player->RandomMove();
	}
}

void PlayerManager::AllPlayerSendMovePacket(){

	msgTest::CS_Player_Move_Request sendPacketPlayerMoveRequest;
	msgTest::MoveState* moveState = sendPacketPlayerMoveRequest.mutable_movestate();
	msgTest::Vector* position = moveState->mutable_position();
	msgTest::Vector* velocity = moveState->mutable_velocity();
	msgTest::Vector* rotation = moveState->mutable_rotation();

	for (auto& p : _players) {

		auto& player = p.second;
		PlayerData playerInfo = player->GetPlayerInfo();

		moveState->set_roomid(playerInfo._enterRoomId);
		moveState->set_playerid(player->GetSessionId());
		position->set_x((int16)(playerInfo._position._x * 100));
		position->set_y((int16)(playerInfo._position._y * 100));
		position->set_z((int16)(playerInfo._position._z * 100));
		velocity->set_x((int16)(playerInfo._velocity._x * 100));
		velocity->set_y((int16)(playerInfo._velocity._y * 100));
		velocity->set_z((int16)(playerInfo._velocity._z * 100));
		rotation->set_x(0);
		rotation->set_y(0);
		rotation->set_z(0);
		//spdlog::info("Send bot{} roomId{} Position ({},{},{}), MoveTime : {}", to_string(player->GetSessionId()), playerInfo._enterRoomId, playerInfo._position._x, playerInfo._position._y, playerInfo._position._z, GGameManager->GetNowServerTimeMs());
		moveState->set_timestamp(GGameManager->GetNowServerTimeMs());

		vector<shared_ptr<Buffer>> sendBuffers = PacketHandler::MakeSendBuffer(sendPacketPlayerMoveRequest, PacketId::PKT_CS_PLAYER_MOVE_REQUEST);

		player->SendData(sendBuffers);
	}
}

void PlayerManager::SendPingPacketToFirstPlayer() {

	const auto& p = _players.begin();
	if (p == _players.end()) {

		spdlog::info("EMPTY PLAYERS");
		return;
	}

	msgTest::CS_Ping sendPacketPing;

	uint64 timestamp = GGameManager->GetNowClientTimeMs();
	sendPacketPing.set_timestamp(timestamp);

	vector<shared_ptr<Buffer>> sendBuffers = PacketHandler::MakeSendBuffer(sendPacketPing, PacketId::PKT_CS_PING);

	SendMsg(p->first, sendBuffers);
}

void PlayerManager::SendMsg(uint64 userId, vector<shared_ptr<Buffer>> sendBuffers){

	const auto& iter = _players.find(userId);
	shared_ptr<Player>& player = iter->second;

	player->SendData(sendBuffers);
}

void PlayerManager::SetEnterRoomId(uint64 userId, int32 roomId) {

	lock_guard<mutex> lock(_playersMutex);
	_players[userId]->SetEnterRoomId(roomId);
}

shared_ptr<Player> PlayerManager::CreatePlayer(const shared_ptr<Session>& playerOwner){

	return make_shared<Player>(playerOwner);
}

void PlayerManager::AddPlayer(const shared_ptr<Player>& player, uint64 userId){

	_playerCount.fetch_add(1);

	lock_guard<mutex> _lock(_playersMutex);
	_players[userId] = player;
}


