#include "pch.h"
#include "Service.h"
#include "BufferPool.h"

Service::Service(ServiceType type, NetAddress address, int32 maxSessionCount) 
	: _serviceType(type), _address(address), _maxSessionCount(maxSessionCount){
	
	SocketManager::SetEnv();
	_completionPortHandler = new CompletionPortHandler();
}

Service::~Service(){

	_sessions.clear();
}

void Service::CompletionEventThread(){

	_completionPortHandler->GetCompletionEvent();
}

void Service::AddSession(shared_ptr<Session> session){

	lock_guard<mutex> _lock(_mutex);

	_curSessionCount++;
	_playerList.insert(_playerId);
	session->SetSessionId(_playerId++);
	_sessions.insert(session);
}

void Service::removeSession(shared_ptr<Session> session){

	lock_guard<mutex> _lock(_mutex);

	_curSessionCount--;
	_sessions.erase(session);
	_playerList.erase(_playerId);
	
	cout << "Current Session Count : " << _sessions.size() << endl;
}

void Service::Broadcast(SendBuffer* sendBuffer){

	lock_guard<mutex> _lock(_mutex);

	for (shared_ptr<Session> session : _sessions) {
		SendBuffer* sendBuf = GSendBufferPool->Pop();
		int32 size = sizeof(SendBuffer);
		memcpy(sendBuf, sendBuffer, size);
		session->Send(sendBuf);
	}

	GSendBufferPool->Push(sendBuffer);
}

void Service::GetUserIdList(int32* array){

	int index = 0;
	for (shared_ptr<Session> session : _sessions) {
		int32 sessionID = session.get()->GetSessionId();
		array[index++] = session.get()->GetSessionId();
	}
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

void ClientService::SendMsg(SendBuffer* sendBuffer){

	for (shared_ptr<Session> session : _sessions) {
		session->Send(sendBuffer);
	}
}

void ClientService::Start(){

	lock_guard<mutex> _lock(_mutex);

	for (int32 i = 0;i < _maxSessionCount;i++) {

		shared_ptr<Session> session = make_shared<Session>(this);
		_completionPortHandler->RegisterHandle((HANDLE)session->GetSocket(), 0);

		_sessions.insert(session);
		session->Connect(_address);
	}
}
