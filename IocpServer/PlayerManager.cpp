#include "pch.h"
#include "PlayerManager.h"


Player::Player(shared_ptr<Session> owner, wstring name, Vector position)
 : _owner(owner), _name(name), _position(position){
	
}

Player::~Player() {
	wcout << L"[REMOVE PLAYER DATA] name :" << _name << endl;
	ClearResource();
}

void Player::ClearResource(){

	_owner = nullptr;
}

PlayerInfo Player::GetPlayerInfo(){

	PlayerInfo playerInfo = {_name, _level, _roomId , _position};
	return playerInfo;
}

void Player::SetInfo(wstring name){
	
	SetName(name);
}

PlayerManager::PlayerManager(){

}

PlayerManager::~PlayerManager(){

}

void PlayerManager::CreateAndAddPlayer(shared_ptr<Session> owner, uint64 sessionId, wstring name, Vector position){
	
	shared_ptr<Player> player = make_shared<Player>(owner, name, position);
	player->SetInfo(name);
	
	{
		lock_guard<mutex> lock(_playersMutex);

		_players[sessionId] = player;
	}
}

shared_ptr<Player> PlayerManager::GetPlayer(uint64 sessionId) {
	
	lock_guard<mutex> lock(_playersMutex);

	return _players[sessionId];
}

void PlayerManager::RemovePlayer(uint64 sessionId){

	shared_ptr<Player> player = GetPlayer(sessionId);

	if (player == nullptr) {
		return;
	}

	lock_guard<mutex> lock(_playersMutex);
	_players.erase(sessionId);
}




