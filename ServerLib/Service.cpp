#include "pch.h"
#include "Service.h"
#include "BufferPool.h"

Service::Service(ServiceType type, NetAddress address, int32 maxSessionCount) 
	: _serviceType(type), _address(address), _maxSessionCount(maxSessionCount){
	
	SocketManager::SetEnv();
	_completionPortHandler = new CompletionPortHandler();
}

void Service::CompletionEventThread(){

	_completionPortHandler->GetCompletionEvent();
}

void Service::AddSession(Session* session){

	lock_guard<mutex> _lock(_mutex);

	_sessions.insert(session);
}

void Service::removeSession(Session* session){

	lock_guard<mutex> _lock(_mutex);

	_sessions.erase(session);
}

void Service::Broadcast(SendBuffer* sendBuffer){

	lock_guard<mutex> _lock(_mutex);

	for (Session* session : _sessions) {
		SendBuffer* sendBuf = GSendBufferPool->Pop();
		int32 size = sizeof(sendBuffer);
		memcpy(sendBuf, sendBuffer, size);
		session->Send(sendBuf);
	}

	GSendBufferPool->Push(sendBuffer);
}

void Service::RegisterHandle(HANDLE handle){

	_completionPortHandler->RegisterHandle(handle);
}

ServerService::ServerService(NetAddress addres, int32 maxSessionCount) 
	: Service(ServiceType::Server, addres, maxSessionCount){

	_listener = new Listener(this);
	_listener->SetEnv(_address);

	ASSERT_CRASH(false == _completionPortHandler->RegisterHandle((HANDLE)_listener->GetSocket(), 0));
}

void ServerService::Start(){

	_listener->Start();
}

ClientService::ClientService(NetAddress address, int32 maxSessionCount) 
	: Service(ServiceType::Client, address, maxSessionCount){

	
}

void ClientService::Start(){

	lock_guard<mutex> _lock(_mutex);

	for (int32 i = 0;i < _maxSessionCount;i++) {

		Session* session = new Session();
		_completionPortHandler->RegisterHandle((HANDLE)session->GetSocket(), 0);

		_sessions.insert(new Session());
		session->Connect(_address);
	}
}
