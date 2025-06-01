#include "pch.h"
#include "PlayerManager.h"
#include "GameSession.h"
#include "RoomManager.h"
#include "JobQueue.h"


Player::Player(shared_ptr<GameSession> owner, PlayerInfo& playerInfo)
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

	lock_guard<mutex> lock(_playerMutex);

	return _playerInfo;
}

void Player::UpdatePlayerInfo(const PlayerInfo& newInfo){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo = newInfo;
}

void Player::SetName(const wstring& name){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._name = name;
}

void Player::SetLevel(int32 level){

	_playerInfo._level = level;
}

void Player::SetRoomId(int32 roomId){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._roomId = roomId;
}

void Player::SetPosition(Vector<int16>& position){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._position = position;
}

void Player::SetVelocity(Vector<int16>& velocity){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._velocity = velocity;
}

void Player::SetMoveTimestamp(int64 timestamp){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._moveTimestamp = timestamp;
}

void Player::UpdateGameState(const GameStateData& gameState){

	lock_guard<mutex> lock(_playerMutex);

	_playerInfo._position = gameState._position;
	_playerInfo._velocity = gameState._velocity;
}

PlayerManager::PlayerManager(){

}

PlayerManager::~PlayerManager(){

}

void PlayerManager::PushCreatePlayerJob(shared_ptr<GameSession> ownerSession, uint64 sessionId, PlayerInfo playerInfo){

	Job* job = new Job([&]() {
		shared_ptr<Player> player = make_shared<Player>(ownerSession, playerInfo);
		_players.insert({ sessionId, player });
	});
	PushJob(job);
}

void PlayerManager::PushRemovePlayerJob(uint64 sessionId){


}

void PlayerManager::PushUpdatePlayerGameStateJob(uint64 sessionId, const Player::GameStateData& gameState){


}





