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

void Player::SetPlayerMove(Vector& position, int64 timestamp){

	lock_guard<mutex> lock(_playerMutex);

	_position = position;
	_moveTimestamp = timestamp;
}

void Player::SetPlayerInfo(PlayerInfo& playerInfo){
	
	lock_guard<mutex> lock(_playerMutex);

	_name = playerInfo._name;
	_roomId = playerInfo._roomId;
	_position = playerInfo._position;
}

void Player::SetName(wstring& name){

	lock_guard<mutex> lock(_playerMutex);

	_name = name;
}

void Player::SetRoomId(int32 roomId){

	lock_guard<mutex> lock(_playerMutex);

	_roomId = roomId;
}

void Player::SetPosition(Vector& position){

	lock_guard<mutex> lock(_playerMutex);

	_position = position;
}

void Player::SetMoveTimestamp(int64 timestamp){

	lock_guard<mutex> lock(_playerMutex);

	_moveTimestamp = timestamp;
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




