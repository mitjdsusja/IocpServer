#include "pch.h"
#include "Player.h"

#include "GameSession.h"
#include "PacketHandler.h"

#include "messageTest.pb.h"


Player::Player(const shared_ptr<Session>& owner) : _owner(owner) {

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

		wcout << L"Empty Players " << endl;
		return;
	}

	const auto& iter = _players.begin();
	auto& playerRef = iter->second;

	vector<shared_ptr<Buffer>> buffer = PacketHandler::MakeSendBuffer(sendPacketRoomListRequest, PacketId::PKT_CS_ROOM_LIST_REQUEST);

	playerRef->SendData(buffer);
}

void PlayerManager::RequestEnterRoomAllPlayer(uint64 roomId){

	msgTest::CS_Enter_Room_Request sendPacketEnterRoomReqeust;
	sendPacketEnterRoomReqeust.set_roomid(roomId);

	vector<shared_ptr<Buffer>> sendBuffers = PacketHandler::MakeSendBuffer(sendPacketEnterRoomReqeust, PacketId::PKT_CS_ENTER_ROOM_REQUEST);

	for (auto& p : _players) {
		auto& player = p.second;

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
