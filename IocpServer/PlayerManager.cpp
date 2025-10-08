﻿#include "pch.h"
#include <string>
#include "Vector.h"
#include "PlayerManager.h"
#include "GameSession.h"
#include "RoomManager.h"
#include "ActorManager.h"
#include "Actor.h"
#include "Job.h"

#include "boost/locale.hpp"

#include "PacketHeader.h"


Player::Player(shared_ptr<GameSession> owner)
 : _owner(owner), Actor(ActorType::PLAYER_TYPE){
	
}

Player::~Player() {

	spdlog::info("[Player::~Player] name : {}", boost::locale::conv::utf_to_utf<char>(_playerData._baseInfo._name));
	ClearResource();
}

void Player::ClearResource(){

	_owner = nullptr;
}

void Player::InitPlayer(const PlayerBaseInfo& baseInfo, const PlayerTransform& transform, const PlayerStats& stats){

	_playerData._baseInfo = baseInfo;
	_playerData._transform = transform;
	_playerData._stats = stats;
}

void Player::PushJobSendData(const shared_ptr<Buffer>& sendBuffer){

	shared_ptr<Player> self = static_pointer_cast<Player>(shared_from_this());
	
	unique_ptr<Job> job = make_unique<Job>([self, sendBuffer]() {
	
		self->SendData(sendBuffer);
	});

	PushJob(move(job));
}

void Player::PushJobUpdatePosition(const PlayerTransform& newPosition){

	const shared_ptr<Player>& self = static_pointer_cast<Player>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, newPosition]() {
		self->UpdateTransform(newPosition);
	});

	PushJob(move(job));
}

void Player::PushJobGetPlayerData(function<void(const PlayerData& playerData)> func){

	shared_ptr<Player> self = static_pointer_cast<Player>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, func]() {

		const PlayerData& playerData = self->_playerData;
		func(playerData);
	});

	PushJob(move(job));
}

void Player::PushJobGetBaseInfo(function<void(PlayerBaseInfo)> func){

	shared_ptr<Player> self = static_pointer_cast<Player>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, func]() {

		PlayerBaseInfo baseInfo =  self->GetBaseInfo();
		func(baseInfo);
	});

	PushJob(move(job));
}

void Player::PushJobGetPosition(function<void(PlayerTransform)> func){

	shared_ptr<Player> self = static_pointer_cast<Player>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, func]() {

		PlayerTransform position = self->GetTransform();
		func(position);
	});

	PushJob(move(job));
}

void Player::PushJobGetStats(function<void(PlayerStats)> func){

	shared_ptr<Player> self = static_pointer_cast<Player>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, func]() {

		PlayerStats stats = self->GetStats();
		func(stats);
	});

	PushJob(move(job));
}

void Player::PushJobSetPosition(const PlayerTransform& position){

	shared_ptr<Player> self = static_pointer_cast<Player>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, position]() {

		self->SetTransform(position);
	});

	this->PushJob(move(job));
}

void Player::SendData(const shared_ptr<Buffer>& sendBuffer){

	_owner->Send(sendBuffer);
}

void Player::UpdateTransform(const PlayerTransform& newTransform){

	_playerData._transform = newTransform;
}

PlayerBaseInfo Player::GetBaseInfo(){

	return _playerData._baseInfo;
}

PlayerTransform Player::GetTransform(){
	
	return _playerData._transform;
}

PlayerStats Player::GetStats(){
	
	return _playerData._stats;
}

void Player::SetTransform(const PlayerTransform& transform){

	if(transform._roomId != 0) _playerData._transform._roomId = transform._roomId;
	if(transform._lastmoveTimestamp != 0) _playerData._transform._lastmoveTimestamp = transform._lastmoveTimestamp;
	if (transform._position.IsZero() == false) {
		_playerData._transform._position = transform._position;
	}
	if (transform._velocity.IsZero() == false) {
		_playerData._transform._velocity = transform._velocity;
	}
}

/*-------------------
	PlayerManager
---------------------*/

PlayerManager::PlayerManager() : Actor(ActorType::PLAYER_MANAGER_TYPE){

}

PlayerManager::~PlayerManager(){

}

void PlayerManager::PushJobSendData(uint64 sessionId, const shared_ptr<Buffer>& sendBuffer){

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, sessionId, sendBuffer]() {
		self->SendData(sessionId, sendBuffer);
	});

	PushJob(move(job));
}

void PlayerManager::PushJobSendData(uint64 sessionId, const vector<shared_ptr<Buffer>>& sendBuffer){

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());
	
	unique_ptr<Job> job = make_unique<Job>([self, sessionId, sendBuffer]() {

		for (auto& buffer : sendBuffer) {

			self->SendData(sessionId, buffer);
		}
	});

	PushJob(move(job));
}

void PlayerManager::PushJobCreateAndPushPlayer(const shared_ptr<GameSession>& ownerSession, const PlayerBaseInfo& baseInfo, const PlayerTransform& position, const PlayerStats& stats) {

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, ownerSession, baseInfo, position, stats]() {
		self->CreateAndPushPlayer(ownerSession, baseInfo, position, stats);
	});

	PushJob(move(job));
}

void PlayerManager::PushJobRemovePlayer(uint64 sessionId){

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, sessionId]() {
		self->RemovePlayer(sessionId);
	});

	PushJob(move(job));
}

void PlayerManager::PushJobGetPlayerData(uint64 sessionId, function<void(const PlayerData& playerData)> func){

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());
	
	unique_ptr<Job> job = make_unique<Job>([self, sessionId, func]() {

		auto iter = self->_players.find(sessionId);
		if (iter == self->_players.end()) {

			spdlog::info("[PlayerManager::PushJobGetPlayerData] Invalid sessionId : {}", sessionId);
			return;
		}

		const shared_ptr<Player>& player = iter->second;
		player->PushJobGetPlayerData(func);
	});
	PushJob(move(job));
}

void PlayerManager::PushJobGetRoomPlayer(uint64 sessionId, function<void(PlayerBaseInfo, PlayerTransform)> func){

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, sessionId, func]() {
		auto iter = self->_players.find(sessionId);
		if (iter == self->_players.end()){

			spdlog::info("[PlayerManager::PushJobGetRoomPlayer] Invalid sessionId : {}", sessionId);
			return;
		}

		const shared_ptr<Player>& player = iter->second;

		// 결과 저장용 변수
		shared_ptr<PlayerBaseInfo> baseInfo = make_shared<PlayerBaseInfo>();
		shared_ptr<PlayerTransform> position = make_shared<PlayerTransform>();
		shared_ptr<atomic<int>> counter = make_shared<atomic<int>>(2);

		// 두 작업이 모두 완료되었을 때 callback 호출
		auto done = [baseInfo, position, counter, func]() {
			if (counter->fetch_sub(1) == 1) {
				func(*baseInfo, *position);
			}
		};

		// Player에게 작업 위임
		player->PushJobGetBaseInfo([baseInfo, done](const PlayerBaseInfo& info) {
			*baseInfo = info;
			done();
		});

		player->PushJobGetPosition([position, done](const PlayerTransform& pos) {
			*position = pos;
			done();
		});
	});

	self->PushJob(move(job));
}

void PlayerManager::PushJobGetBaseInfo(uint64 sessionId, function<void(PlayerBaseInfo)> func) {

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, sessionId, func]() {

		auto iter = self->_players.find(sessionId);
		if (iter == self->_players.end()) {

			spdlog::info("[PlayerManager::PushJobGetRoomPlayer] Invalid sessionId : {}", sessionId);
			return;
		}

		const shared_ptr<Player>& player = iter->second;

		player->PushJobGetBaseInfo(func);
	});

	PushJob(move(job));
}

void PlayerManager::PushJobGetPosition(uint64 sessionId, function<void(PlayerTransform)> func) {

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, sessionId, func]() {

		auto iter = self->_players.find(sessionId);
		if (iter == self->_players.end()) {

			spdlog::info("[PlayerManager::PushJobGetPosition] Invalid sessionId : {}", sessionId);
			//cout << "[PlayerManager::PushJobGetPosition] Invalid sessionId : " << sessionId << endl;
			return;
		}

		const shared_ptr<Player>& player = iter->second;

		player->PushJobGetPosition(func);
		});

	PushJob(move(job));
}

void PlayerManager::PushJobGetstats(uint64 sessionId, function<void(PlayerStats)> func) {

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, sessionId, func]() {

		auto iter = self->_players.find(sessionId);
		if (iter == self->_players.end()) {

			spdlog::info("[PlayerManager::PushJobGetPosition] Invalid sessionId : {}", sessionId);
			//cout << "[PlayerManager::PushJobGetPosition] Invalid sessionId : " << sessionId << endl;
			return;
		}

		const shared_ptr<Player>& player = iter->second;

		player->PushJobGetStats(func);
	});

	PushJob(move(job));
}

void PlayerManager::PushJobSetPosition(uint64 sessionId, PlayerTransform position){

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, sessionId, position]() {
		
		self->SetTransform(sessionId, position);
	});

	this->PushJob(move(job));
}

void PlayerManager::SendData(uint64 sessionId, const shared_ptr<Buffer>& sendBuffer) {

	const auto& p = _players.find(sessionId);
	if (p == _players.end()) {

		spdlog::info("[PlayerManager::SendData] Invalid Player : {}", sessionId);
		//cout << "[PlayerManager::SendData] Invalid Player : " << sessionId << endl;
		return;
	}

	shared_ptr<Player>& player = p->second;

	player->PushJobSendData(sendBuffer);
}

void PlayerManager::CreateAndPushPlayer(const shared_ptr<GameSession>& ownerSession, const PlayerBaseInfo& baseInfo, const PlayerTransform& position, const PlayerStats& stats){

	shared_ptr<Player> player = make_shared<Player>(ownerSession);
	player->SetActorId(GActorManager->RegisterActor(player));
	player->InitPlayer(baseInfo, position, stats);

	_players.insert({ baseInfo._sessionId, player });

	spdlog::info("[PlayerManager::CreateAndPushPlayer] Create Player : {}", boost::locale::conv::utf_to_utf<char>(baseInfo._name));
	//wcout << L"[PlayerManager::CreateAndPushPlayer] Create Player : " << baseInfo._name << endl;
}

void PlayerManager::RemovePlayer(uint64 sessionId) {

	const auto& p = _players.find(sessionId);
	if (_players.end() == p) {

		spdlog::info("[PlayerManager::RemovePlayer] Invalid SessionId : {}", sessionId);
		//cout << "[PlayerManager::RemovePlayer] Invalid SessionId : " << sessionId << endl;
		return;
	}

	shared_ptr<Player> player = p->second;

	// DB에 PlayerData 저장
	// 추가 예정

	_players.erase(sessionId);
	GActorManager->UnRegisterActor(player->GetActorId());
}

void PlayerManager::SetTransform(uint64 sessionId, const PlayerTransform& position){

	const auto& p = _players.find(sessionId);
	if (p == _players.end()) {

		spdlog::info("[PlayerManager::SetPosition] Invalid sessionId : {}", sessionId);
		//wcout << "[PlayerManager::SetPosition] Invalid sessionId : " << sessionId << endl;
		return;
	}

	const shared_ptr<Player>& player = p->second;

	player->PushJobSetPosition(position);
}




