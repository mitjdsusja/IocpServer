#include "pch.h"
#include "PlayerManager.h"
#include "GameSession.h"
#include "RoomManager.h"


Player::Player(shared_ptr<GameSession> owner, PlayerInfo playerInfo)
 : _owner(owner), _playerInfo(playerInfo){
	
}

Player::~Player() {

	_joinedRoom = nullptr;

	wcout << L"[REMOVE PLAYER DATA] name :" << _playerInfo._name << endl;
	ClearResource();
}

void Player::ClearResource(){

	_owner = nullptr;
}

PlayerInfo Player::GetPlayerInfo(){

	lock_guard<mutex> lock(_playerMutex);

	PlayerInfo info = _playerInfo;
	_playerInfo._isInfoUpdated = false;
	return info;
}

shared_ptr<Room> Player::GetJoinedRoom(){

	lock_guard<mutex> lock(_playerMutex);

	return _joinedRoom;
}

void Player::SetPlayerMove(PlayerInfo& playerInfo){

	{
		lock_guard<mutex> lock(_playerMutex);

		_playerInfo._position = playerInfo._position;
		_playerInfo._velocity = playerInfo._velocity;
		_playerInfo._moveTimestamp = playerInfo._moveTimestamp;
		_playerInfo._isInfoUpdated = true;
	}
}

void Player::SetJoinedRoom(shared_ptr<Room> room){

	lock_guard<mutex> lock(_playerMutex);

	if (_joinedRoom != nullptr) {
		cout << "이미 방에 들어와 있습니다." << endl;
		return;
	}
	
	_joinedRoom = room;
}

void Player::SetPlayerInfo(PlayerInfo& playerInfo) {

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo = playerInfo;
	_playerInfo._isInfoUpdated = true;
}

void Player::SetName(wstring& name){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._name = name;
	_playerInfo._isInfoUpdated = true;
}

void Player::SetRoomId(int32 roomId){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._roomId = roomId;
	_playerInfo._isInfoUpdated = true;
}

void Player::SetPosition(Vector<int16>& position){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._position = position;
	_playerInfo._isInfoUpdated = true;
}

void Player::SetVelocity(Vector<int16>& velocity){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._velocity = velocity;
	_playerInfo._isInfoUpdated = true;
}

void Player::SetMoveTimestamp(int64 timestamp){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._moveTimestamp = timestamp;
	_playerInfo._isInfoUpdated = true;
}

PlayerManager::PlayerManager(){

}

PlayerManager::~PlayerManager(){

}

void PlayerManager::CreateAndAddPlayer(shared_ptr<GameSession> owner, uint64 sessionId){

	PlayerInfo playerInfo = {};

	CreateAndAddPlayer(owner, sessionId, playerInfo);
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




