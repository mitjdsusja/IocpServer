#include "pch.h"
#include "Player.h"
#include "Global.h"

#include "GameSession.h"
#include "PacketHandler.h"
#include "GameManager.h"

#include "messageTest.pb.h"

const float MAP_MIN_X = -2000.0f;
const float MAP_MAX_X = 2000.0f;
const float MAP_MIN_Z = -2000.0f;
const float MAP_MAX_Z = 2000.0f;

Player::Player(const shared_ptr<Session>& owner) : _owner(owner) {

	_playerInfo._position = { 0, 100, 0 };
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

	// 경계 체크 후 반전
	if (newX < MAP_MIN_X || newX > MAP_MAX_X) {
		_playerInfo._velocity._x = -_playerInfo._velocity._x;
		dx = -dx;
	}
	if (newZ < MAP_MIN_Z || newZ > MAP_MAX_Z) {
		_playerInfo._velocity._z= -_playerInfo._velocity._z;
		dz = -dz;
	}

	_playerInfo._position._x += static_cast<int16>(dx);
	_playerInfo._position._z += static_cast<int16>(dz);
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
