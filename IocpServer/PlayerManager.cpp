#include "pch.h"
#include "PlayerManager.h"
#include "GameSession.h"


Player::Player(shared_ptr<GameSession> owner, PlayerInfo playerInfo)
 : _owner(owner), _playerInfo(playerInfo){
	
}

Player::~Player() {
	wcout << L"[REMOVE PLAYER DATA] name :" << _playerInfo._name << endl;
	ClearResource();
}

void Player::ClearResource(){

	_owner = nullptr;
}

PlayerInfo Player::GetPlayerInfo(){

	return _playerInfo;
}

void Player::SetPlayerMove(Vector& position, int64 timestamp){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._position = position;
	_playerInfo._moveTimestamp = timestamp;
}

void Player::SetPlayerInfo(PlayerInfo& playerInfo){
	
	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._name = playerInfo._name;
	_playerInfo._roomId = playerInfo._roomId;
	_playerInfo._position = playerInfo._position;
}

void Player::SetName(wstring& name){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._name = name;
}

void Player::SetRoomId(int32 roomId){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._roomId = roomId;
}

void Player::SetPosition(Vector& position){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._position = position;
}

void Player::SetMoveTimestamp(int64 timestamp){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._moveTimestamp = timestamp;
}

PlayerManager::PlayerManager(){

}

PlayerManager::~PlayerManager(){

}

void PlayerManager::CreateAndAddPlayer(shared_ptr<GameSession> owner, uint64 sessionId, PlayerInfo playerInfo){
	
	lock_guard<mutex> lock(_playersMutex);

	auto it = _players.find(sessionId);
	if (it != _players.end()) {
		cout << "[VALID PLAYER] session Id : " << sessionId << endl;
		return;
	}

	shared_ptr<Player> player = make_shared<Player>(owner, playerInfo);

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

void PlayerManager::SetPlayerInfo(int64 sessionId, PlayerInfo& playerInfo){

	shared_ptr<Player> player = GetPlayer(sessionId);
	player->SetPlayerInfo(playerInfo);
}

void PlayerManager::RemovePlayer(uint64 sessionId){

	lock_guard<mutex> lock(_playersMutex);

	auto it = _players.find(sessionId);
	if (it == _players.end()) {
		return;
	}

	_players.erase(sessionId);
}




