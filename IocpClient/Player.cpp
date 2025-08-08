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

	_playerInfo._position = { 0, 100, 0 };
	_playerInfo._velocity = { 0, 0, 0 };
}

void Player::RandomMove() {

	using namespace std::chrono;

	static thread_local std::mt19937 rng(std::random_device{}());
	static thread_local std::uniform_real_distribution<float> dirDist(-1.0f, 1.0f);

	float dirX = dirDist(rng);
	float dirZ = dirDist(rng);

	float length = std::sqrt(dirX * dirX + dirZ * dirZ);
	if (length == 0.0f)
		return;

	dirX /= length;
	dirZ /= length;

	int16 speed = _playerInfo._moveSpeed;

	// 이동 시간(프레임마다 호출된다고 가정, 0.1초 주기로)
	float deltaTime = 0.1f;
	int16 dx = dirX * speed * deltaTime;
	int16 dz = dirZ * speed * deltaTime;

	int16 newX = _playerInfo._position._x + dx;
	int16 newZ = _playerInfo._position._z + dz;

	// 맵 범위 확인 및 방향 반전
	if (newX < MAP_MIN_X || newX > MAP_MAX_X)
		dx = -dx;
	if (newZ < MAP_MIN_Z || newZ > MAP_MAX_Z)
		dz = -dz;

	_playerInfo._position._x += dx;
	_playerInfo._position._z += dz;
}

void Player::SendData(const vector<shared_ptr<Buffer>>& sendBuffer) {

	for (auto& buffer : sendBuffer) {

		_owner->Send(buffer);
	}
}

uint64 Player::GetSessionId() {

	return _owner->GetSessionId();
}

PlayerInfo Player::GetPlayerInfo(){

	return _playerInfo;
}

void PlayerManager::CreatePlayerAndAdd(const shared_ptr<Session>& playerOwner, uint64 userId){

	shared_ptr<Player> player = CreatePlayer(playerOwner);

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

void PlayerManager::AllPlayerRequestEnterRoom(uint64 roomId){

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
		PlayerInfo playerInfo = player->GetPlayerInfo();

		moveState->set_roomid(GGameManager->GetEnterRoomId());
		moveState->set_playername("bot" + to_string(player->GetSessionId()));
		position->set_x(playerInfo._position._x);
		position->set_y(playerInfo._position._y);
		position->set_z(playerInfo._position._z);
		velocity->set_x(playerInfo._velocity._x);
		velocity->set_y(playerInfo._velocity._y);
		velocity->set_z(playerInfo._velocity._z);
		rotation->set_x(0);
		rotation->set_y(0);
		rotation->set_z(0);
		moveState->set_timestamp(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());

		vector<shared_ptr<Buffer>> sendBuffers = PacketHandler::MakeSendBuffer(sendPacketPlayerMoveRequest, PacketId::PKT_CS_PLAYER_MOVE_REQUEST);

		player->SendData(sendBuffers);
	}
}

void PlayerManager::SendMsg(uint64 userId, vector<shared_ptr<Buffer>> sendBuffers){

	const auto& iter = _players.find(userId);
	shared_ptr<Player>& player = iter->second;

	player->SendData(sendBuffers);
}

shared_ptr<Player> PlayerManager::CreatePlayer(const shared_ptr<Session>& playerOwner){

	return make_shared<Player>(playerOwner);
}

void PlayerManager::AddPlayer(const shared_ptr<Player>& player, uint64 userId){

	_playerCount.fetch_add(1);
	_players[userId] = player;
}

void PlayerManager::PlayerRandomMove(){


}
