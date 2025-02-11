#include "pch.h"
#include "PlayerManager.h"

Player::Player(int32 userId, wstring name) : _userId(userId), _name(name) {

}

Player::Player(shared_ptr<Session> owner, int32 userId, wstring name)
 : _owner(owner), _userId(userId), _name(name){
	
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

void PlayerManager::CreateAndAddPlayer(shared_ptr<Session> owner, uint64 sessionId, wstring name, int32 userId){
	
	shared_ptr<Player> player = make_shared<Player>(owner, userId, name);
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


