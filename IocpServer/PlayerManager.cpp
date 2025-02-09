#include "pch.h"
#include "PlayerManager.h"

Player::Player() {

}

Player::Player(shared_ptr<Session> owner){

}

Player::~Player() {

}

void Player::SetInfo(wstring name){
	
	SetName(name);
}

PlayerManager::PlayerManager(){

}

PlayerManager::~PlayerManager(){

}

void PlayerManager::CreateAndAddPlayer(shared_ptr<Session> owner, uint64 sessionId, wstring name){
	
	shared_ptr<Player> player = make_shared<Player>(owner);
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


