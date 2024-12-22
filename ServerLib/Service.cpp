#include "pch.h"
#include "Service.h"
#include "BufferPool.h"
#include "PacketHandler.h"

Service::Service(ServiceType type, NetAddress address, int32 maxSessionCount) 
	: _serviceType(type), _address(address), _maxSessionCount(maxSessionCount){
	
	SocketManager::SetEnv();
	_completionPortHandler = new CompletionPortHandler();
}

Service::~Service(){

	_sessions.clear();
}

void Service::CompletionEventThread(uint32 ms){

	_completionPortHandler->GetCompletionEvent(ms);
}

void Service::AddSession(shared_ptr<Session> session){

	lock_guard<mutex> _lock(_mutex);

	_curSessionCount++;
	cout << "Add Session : " << _curSessionCount << endl;
	_playerList.insert(_playerId);
	session->SetSessionId(_playerId++);
	_sessions.insert(pair<int, shared_ptr<Session>>(session->GetSessionId(), session));
}

void Service::removeSession(shared_ptr<Session> session){

	lock_guard<mutex> _lock(_mutex);

	_curSessionCount--;
	_sessions.erase(session->GetSessionId());
	_playerList.erase(_playerId);
	
	cout << "Current Session Count : " << _sessions.size() << endl;
}

void Service::Broadcast(Buffer* sendBuffer){

	lock_guard<mutex> _lock(_mutex);

	int32 sendLen = sendBuffer->WriteSize();

	for (pair<int, shared_ptr<Session>> sessionData : _sessions) {
		Buffer* sendBuf = LSendBufferPool->Pop();
		memcpy(sendBuf->GetBuffer(), sendBuffer->GetBuffer(), sendLen);
		sendBuf->Write(sendLen);
		sessionData.second->Send(sendBuf);
	}

	LSendBufferPool->Push(sendBuffer);
}

void Service::GetUserIdList(int32* array){

	int index = 0;
	for (pair<int, shared_ptr<Session>> sessionData : _sessions) {
		int32 sessionID = sessionData.second.get()->GetSessionId();
		array[index++] = sessionData.second.get()->GetSessionId();
	}
}

void Service::GetUsersInfo(vector<UserInfo*>& userInfoList){

	lock_guard<mutex> lock(_mutex);

	for (const auto& sessionData : _sessions) {
		userInfoList.push_back(&sessionData.second->GetUserInfo());
	}
}

void Service::SetUserInfo(UserInfo srcUserInfo){

	lock_guard<mutex> lock(_mutex);

	_sessions[(srcUserInfo.GetId())]->SetUserPosition(srcUserInfo.GetPosition().x, srcUserInfo.GetPosition().y, srcUserInfo.GetPosition().z);
	_sessions[(srcUserInfo.GetId())]->SetUserVelocity(srcUserInfo.GetVelocity().x, srcUserInfo.GetVelocity().y, srcUserInfo.GetVelocity().z);
}

void Service::RegisterHandle(HANDLE handle){

	_completionPortHandler->RegisterHandle(handle);
}

ServerService::ServerService(NetAddress addres, int32 maxSessionCount) 
	: Service(ServiceType::Server, addres, maxSessionCount){

	_listener = make_shared<Listener>(this);
	_listener->SetEnv(_address);

	ASSERT_CRASH(false == _completionPortHandler->RegisterHandle((HANDLE)_listener->GetSocket(), 0));
}

void ServerService::Start(){

	_listener->Start();
}

ClientService::ClientService(NetAddress address, int32 maxSessionCount) 
	: Service(ServiceType::Client, address, maxSessionCount){

	
}

void ClientService::SendMsg(Buffer* sendBuffer){

	for (pair<int, shared_ptr<Session>> sessionData : _sessions) {
		sessionData.second->Send(sendBuffer);
	}
}

void ClientService::Start(){

	lock_guard<mutex> _lock(_mutex);

	for (int32 i = 0;i < _maxSessionCount;i++) {

		shared_ptr<ServerSession> session = make_shared<ServerSession>(this);
		_completionPortHandler->RegisterHandle((HANDLE)session->GetSocket(), 0);

		_sessions.insert(pair<int, shared_ptr<Session>>(session->GetSessionId(), session));
		session->Connect(_address);
	}
}
