#include "pch.h"
#include "PlayerManager.h"
#include "GameSession.h"


Player::Player(shared_ptr<GameSession> owner, wstring name, Vector position)
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

void Player::SetPlayerInfo(PlayerInfo& playerInfo){
	
	lock_guard<mutex> lock(_playerMutex);

	_name = playerInfo._name;
	_roomId = playerInfo._roomId;
}

void Player::SetName(wstring& name){

	lock_guard<mutex> lock(_playerMutex);

	_name = name;
}

void Player::SetRoomId(int32 roomId){

	lock_guard<mutex> lock(_playerMutex);

	_roomId = roomId;
}

PlayerManager::PlayerManager(){

}

PlayerManager::~PlayerManager(){

}

void PlayerManager::CreateAndAddPlayer(shared_ptr<GameSession> owner, uint64 sessionId, wstring name, Vector position){
	
	lock_guard<mutex> lock(_playersMutex);

	auto it = _players.find(sessionId);
	if (it != _players.end()) {
		cout << "[VALID PLAYER] session Id : " << sessionId << endl;
		return;
	}

	shared_ptr<Player> player = make_shared<Player>(owner, name, position);

	_players[sessionId] = player;
}

shared_ptr<Player> PlayerManager::GetPlayer(uint64 sessionId) {
	
	lock_guard<mutex> lock(_playersMutex);

	auto it = _players.find(sessionId);
	if (it == _players.end()) {
		return nullptr;
	}

	return it->second;
}

void PlayerManager::RemovePlayer(uint64 sessionId){

	lock_guard<mutex> lock(_playersMutex);

	auto it = _players.find(sessionId);
	if (it == _players.end()) {
		return;
	}

	_players.erase(sessionId);
}




