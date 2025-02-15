#include "pch.h"
#include "PlayerManager.h"


Player::Player(shared_ptr<Session> owner, wstring name)
 : _owner(owner), _name(name){
	
}

Player::~Player() {

}

void Player::ClearResource(){

	_owner = nullptr;
}

void Player::SetInfo(wstring name){
	
	SetName(name);
}

PlayerManager::PlayerManager(){

}

PlayerManager::~PlayerManager(){

}

void PlayerManager::CreateAndAddPlayer(shared_ptr<Session> owner, uint64 sessionId, wstring name){
	
	shared_ptr<Player> player = make_shared<Player>(owner, name);
	player->SetInfo(name);
	
	{
		lock_guard<mutex> lock(_playersMutex);

		players[sessionId] = player;
	}
}

shared_ptr<Player> PlayerManager::GetPlayer(uint64 sessionId) {
	
	lock_guard<mutex> lock(_playersMutex);

	return players[sessionId];
}


