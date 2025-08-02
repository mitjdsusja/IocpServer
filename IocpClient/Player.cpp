#include "pch.h"
#include "Player.h"

#include "GameSession.h"

Player::Player(shared_ptr<Session> owner) : _owner(owner) {

}

uint64 Player::GetSessionId() {

	return _owner->GetSessionId();
}

void PlayerManager::CreatePlayerAndAdd(const shared_ptr<Session>& playerOwner){

	shared_ptr<Player> player = CreatePlayer(playerOwner);

	AddPlayer(player);
}

void PlayerManager::LoginRequest(){
	
}

shared_ptr<Player> PlayerManager::CreatePlayer(const shared_ptr<Session>& playerOwner){

	return make_shared<Player>(playerOwner);
}

void PlayerManager::AddPlayer(shared_ptr<Player>& player){

	_players[player->GetSessionId()] = player;
}
