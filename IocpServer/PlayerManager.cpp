#include "pch.h"
#include <string>
#include "Vector.h"
#include "PlayerManager.h"
#include "GameSession.h"
#include "RoomManager.h"
#include "ActorManager.h"
#include "Actor.h"
#include "Job.h"


Player::Player(shared_ptr<GameSession> owner)
 : _owner(owner){
	
}

Player::~Player() {

	wcout << L"[Player::~Player] name :" << _info._baseInfo._name << endl;
	ClearResource();
}

void Player::ClearResource(){

	_owner = nullptr;
}

void Player::InitPlayer(const PlayerBaseInfo& baseInfo, const PlayerPosition& position, const PlayerStats& stats){

	_info._baseInfo = baseInfo;
	_info._position = position;
	_info._stats = stats;
}

void Player::PushJobSendData(const shared_ptr<Buffer>& sendBuffer){

	shared_ptr<Player> self = static_pointer_cast<Player>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, sendBuffer]() {
		self->SendData(sendBuffer);
	});

	PushJob(move(job));
}

void Player::PushJobUpdatePosition(const PlayerPosition& newPosition){

	const shared_ptr<Player>& self = static_pointer_cast<Player>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, newPosition]() {
		self->UpdatePosition(newPosition);
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

void Player::PushJobGetPosition(function<void(PlayerPosition)> func){

	shared_ptr<Player> self = static_pointer_cast<Player>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, func]() {

		PlayerPosition position = self->GetPosition();
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

void Player::PushJobSetPosition(const PlayerPosition& position){

	shared_ptr<Player> self = static_pointer_cast<Player>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, position]() {

		self->SetPosition(position);
	});

	this->PushJob(move(job));
}

void Player::SendData(const shared_ptr<Buffer>& sendBuffer){

	_owner->Send(sendBuffer);
}

void Player::UpdatePosition(const PlayerPosition& newPosition){

	_info._position = newPosition;
}

PlayerBaseInfo Player::GetBaseInfo(){

	return _info._baseInfo;
}

PlayerPosition Player::GetPosition(){
	
	return _info._position;
}

PlayerStats Player::GetStats(){
	
	return _info._stats;
}

void Player::SetPosition(const PlayerPosition& position){

	if(position._roomId != 0) _info._position._roomId = position._roomId;
	if(position._moveTimestamp != 0) _info._position._moveTimestamp = position._moveTimestamp;
	if (position._position.IsZero() == false) {
		_info._position._position = position._position;
	}
	if (position._velocity.IsZero() == false) {
		_info._position._velocity = position._velocity;
	}
}

/*-------------------
	PlayerManager
---------------------*/

PlayerManager::PlayerManager(){

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

void PlayerManager::PushJobCreateAndPushPlayer(const shared_ptr<GameSession>& ownerSession, const PlayerBaseInfo& baseInfo, const PlayerPosition& position, const PlayerStats& stats) {

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

void PlayerManager::PushJobGetRoomPlayer(uint64 sessionId, function<void(PlayerBaseInfo, PlayerPosition)> func){

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, sessionId, func]() {
		auto iter = self->_players.find(sessionId);
		if (iter == self->_players.end()){

			cout << "[PlayerManager::PushJobGetRoomPlayer] Invalid sessionId : " << sessionId << endl;
			return;
		}

		const shared_ptr<Player>& player = iter->second;

		// 결과 저장용 변수
		shared_ptr<PlayerBaseInfo> baseInfo = make_shared<PlayerBaseInfo>();
		shared_ptr<PlayerPosition> position = make_shared<PlayerPosition>();
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

		player->PushJobGetPosition([position, done](const PlayerPosition& pos) {
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

			cout << "[PlayerManager::PushJobGetRoomPlayer] Invalid sessionId : " << sessionId << endl;
			return;
		}

		const shared_ptr<Player>& player = iter->second;

		player->PushJobGetBaseInfo(func);
	});

	PushJob(move(job));
}

void PlayerManager::PushJobGetPosition(uint64 sessionId, function<void(PlayerPosition)> func) {

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, sessionId, func]() {

		auto iter = self->_players.find(sessionId);
		if (iter == self->_players.end()) {

			cout << "[PlayerManager::PushJobGetPosition] Invalid sessionId : " << sessionId << endl;
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

			cout << "[PlayerManager::PushJobGetPosition] Invalid sessionId : " << sessionId << endl;
			return;
		}

		const shared_ptr<Player>& player = iter->second;

		player->PushJobGetStats(func);
	});

	PushJob(move(job));
}

void PlayerManager::PushJobSetPosition(uint64 sessionId, PlayerPosition position){

	shared_ptr<PlayerManager> self = static_pointer_cast<PlayerManager>(shared_from_this());

	unique_ptr<Job> job = make_unique<Job>([self, sessionId, position]() {
		
		self->SetPosition(sessionId, position);
	});

	this->PushJob(move(job));
}

void PlayerManager::SendData(uint64 sessionId, const shared_ptr<Buffer>& sendBuffer) {

	const auto& p = _players.find(sessionId);
	if (p == _players.end()) {
		cout << "[PlayerManager::SendData] Invalid Player : " << sessionId << endl;
		return;
	}

	shared_ptr<Player>& player = p->second;

	player->PushJobSendData(sendBuffer);
}

void PlayerManager::CreateAndPushPlayer(const shared_ptr<GameSession>& ownerSession, const PlayerBaseInfo& baseInfo, const PlayerPosition& position, const PlayerStats& stats){

	shared_ptr<Player> player = make_shared<Player>(ownerSession);
	player->SetActorId(GActorManager->RegisterActor(player));
	player->InitPlayer(baseInfo, position, stats);

	_players.insert({ baseInfo._sessionId, player });

	wcout << L"[PlayerManager::CreateAndPushPlayer] Create Player : " << baseInfo._name << endl;
}

void PlayerManager::RemovePlayer(uint64 sessionId) {

	const auto& p = _players.find(sessionId);
	if (_players.end() == p) {
		cout << "[PlayerManager::RemovePlayer] Invalid SessionId : " << sessionId << endl;
		return;
	}

	shared_ptr<Player> player = p->second;

	// DB에 PlayerData 저장
	// 추가 예정

	_players.erase(sessionId);
}

void PlayerManager::SetPosition(uint64 sessionId, const PlayerPosition& position){

	const auto& p = _players.find(sessionId);
	if (p == _players.end()) {

		wcout << "[PlayerManager::SetPosition] Invalid sessionId : " << sessionId << endl;
		return;
	}

	const shared_ptr<Player>& player = p->second;

	player->PushJobSetPosition(position);
}




