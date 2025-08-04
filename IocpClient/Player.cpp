#include "pch.h"
#include "Player.h"

#include "GameSession.h"
#include "PacketHandler.h"


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

shared_ptr<Player> PlayerManager::CreatePlayer(const shared_ptr<Session>& playerOwner){

	return make_shared<Player>(playerOwner);
}

void PlayerManager::AddPlayer(shared_ptr<Player>& player){

	_playerCount.fetch_add(1);
	_players[player->GetSessionId()] = player;
}
