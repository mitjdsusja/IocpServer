#include "pch.h"
#include "Player.h"

#include "GameSession.h"
#include "PacketHandler.h"
#include "messageTest.pb.h"

Player::Player(shared_ptr<Session> owner) : _owner(owner) {

}

void Player::SendData(const vector<shared_ptr<Buffer>>& sendBuffer) {

	for (auto& buffer : sendBuffer) {

		_owner->Send(buffer);
	}
}

uint64 Player::GetSessionId() {

	return _owner->GetSessionId();
}

void PlayerManager::CreatePlayerAndAdd(const shared_ptr<Session>& playerOwner){

	shared_ptr<Player> player = CreatePlayer(playerOwner);

	AddPlayer(player);
}

void PlayerManager::LoginRequest(){
	
	msgTest::CS_Login_Request sendPacketLoginrequest;

	int count = 1;
	for (auto& p : _players) {

		auto& player = p.second;
		sendPacketLoginrequest.set_id("bot" + count);
		sendPacketLoginrequest.set_password("bot" + count);

		++count;

		const vector<shared_ptr<Buffer>> sendBuffer = PacketHandler::MakeSendBuffer(sendPacketLoginrequest, PacketId::PKT_CS_LOGIN_REQUEST);

		player->SendData(sendBuffer);
	}
}

shared_ptr<Player> PlayerManager::CreatePlayer(const shared_ptr<Session>& playerOwner){

	return make_shared<Player>(playerOwner);
}

void PlayerManager::AddPlayer(shared_ptr<Player>& player){

	_playerCount.fetch_add(1);
	_players[player->GetSessionId()] = player;
}
