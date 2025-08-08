#include "pch.h"
#include "Player.h"

#include "GameSession.h"
#include "PacketHandler.h"

#include "messageTest.pb.h"


Player::Player(const shared_ptr<Session>& owner) : _owner(owner) {

	_playerInfo._position = { 0, 100, 0 };
	_playerInfo._velocity = { 0, 0, 0 };
}

void Player::RandomMove() {

	auto now = chrono::system_clock::now();

	// 1. 시간 경과 계산
	float deltaTime = chrono::duration_cast<chrono::duration<float>>(now - _playerInfo.lastCalculatedTimePoint).count();

	// 2. 이전 velocity에 따라 현재 위치 업데이트
	_playerInfo._position._x += static_cast<int16>(round(_playerInfo._velocity._x * deltaTime));
	_playerInfo._position._z += static_cast<int16>(round(_playerInfo._velocity._z * deltaTime));

	// 3. 랜덤 방향 설정
	static random_device rd;
	static mt19937 gen(rd());
	uniform_real_distribution<float> dist(-1.0f, 1.0f);

	float dirX = dist(gen);
	float dirZ = dist(gen);

	float length = sqrt(dirX * dirX + dirZ * dirZ);
	if (length == 0.0f)
		return; // 제자리 방향이면 무시

	dirX /= length;
	dirZ /= length;

	// 4. 속도 계산
	int16 speed = _playerInfo._moveSpeed; // 초당 이동거리
	_playerInfo._velocity._x = static_cast<int16>(round(dirX * speed));
	_playerInfo._velocity._z = static_cast<int16>(round(dirZ * speed));

	// 5. 마지막 시간 갱신
	_playerInfo.lastCalculatedTimePoint = now;
}

void Player::SendData(const vector<shared_ptr<Buffer>>& sendBuffer) {

	for (auto& buffer : sendBuffer) {

		_owner->Send(buffer);
	}
}

uint64 Player::GetSessionId() {

	return _owner->GetSessionId();
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


}

void PlayerManager::AllPlayerSendMovePacket(){


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
