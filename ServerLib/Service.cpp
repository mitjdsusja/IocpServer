#include "pch.h"
#include "Service.h"
#include "BufferPool.h"
#include "PacketHandler.h"

Service::Service(ServiceType type, NetAddress address, int32 maxSessionCount, function<shared_ptr<Session>(void)> sessionCreateFunc) 
	: _serviceType(type), _address(address), _maxSessionCount(maxSessionCount), _sessionCreateFunc(sessionCreateFunc){
	
	SocketManager::SetEnv();

	_completionPortHandler = new CompletionPortHandler();
}

Service::~Service(){

	_sessions.clear();
}

void Service::CompletionEventThread(uint32 ms){

	_completionPortHandler->GetCompletionEvent(ms);
}

shared_ptr<Session> Service::CreateSession(){

	return _sessionCreateFunc();
}

void Service::AddSession(shared_ptr<Session> session){

	lock_guard<mutex> _lock(_sessionsMutex);
	
	_curSessionCount++;
	cout << "Add Session : " << _curSessionCount << endl;

	session->SetSessionId(GenerateSessionId());
	
	_sessions[session->GetSessionId()] = session;
}

uint64 Service::GenerateSessionId(){

	uint64 timePart = (uint64)chrono::steady_clock::now().time_since_epoch().count();
	
	return timePart;
}

void Service::removeSession(shared_ptr<Session> session){

	lock_guard<mutex> _lock(_sessionsMutex);

	_curSessionCount--;

	_sessions.erase(session->GetSessionId());
}

void Service::Broadcast(shared_ptr<Buffer> sendDataBuffer){

	map<int,shared_ptr<Session>> sessionsCopy;
	{
		lock_guard<mutex> _lock(_sessionsMutex);
		sessionsCopy = _sessions;
	}

	for (const auto& [id,session] : sessionsCopy) {
		session->Send(sendDataBuffer);
	}

	//int32 sendLen = sendDataBuffer->WriteSize();
	//for (const auto& [id, session] : _sessions) {
	//	shared_ptr<Buffer> sendBuffer = shared_ptr<Buffer>(GSendBufferPool->Pop(), [](Buffer* buffer) { GSendBufferPool->Push(buffer); });
	//	// TODO : copy operator
	//	memcpy(sendBuffer->GetBuffer(), sendDataBuffer->GetBuffer(), sendLen);
	//	sendBuffer->Write(sendLen);
	//	session->Send(sendBuffer);
	//}
}

void Service::GetUsersInfo(vector<UserInfo>& userInfoList){

	lock_guard<mutex> lock(_sessionsMutex);

	for (const auto& [id, session] : _sessions) {
		userInfoList.push_back(session->GetUserInfo());
	}
}

void Service::SetUserInfo(UserInfo srcUserInfo){

	lock_guard<mutex> lock(_sessionsMutex);

	shared_ptr<Session> session = _sessions[srcUserInfo.GetId()];
	
	session->SetUserInfo(srcUserInfo);
}

void Service::RegisterHandle(HANDLE handle){

	_completionPortHandler->RegisterHandle(handle);
}

ServerService::ServerService(NetAddress addres, int32 maxSessionCount, function<shared_ptr<Session>(void)> sessionCreateFunc) 
	: Service(ServiceType::Server, addres, maxSessionCount, sessionCreateFunc){

	_listener = make_shared<Listener>(this);
	_listener->Init(_address);

	ASSERT_CRASH(false == _completionPortHandler->RegisterHandle((HANDLE)_listener->GetSocket(), 0));
}

void ServerService::Start(){

	_listener->Start();
}

ClientService::ClientService(NetAddress address, int32 maxSessionCount, function<shared_ptr<Session>(void)> sessionCreateFunc)
	: Service(ServiceType::Client, address, maxSessionCount, sessionCreateFunc){

	
}

void ClientService::SendMsg(shared_ptr<Buffer> sendBuffer){

	for (const auto& [id, session] : _sessions) {
		session->Send(sendBuffer);
	}
}

void ClientService::Start(){

	lock_guard<mutex> _lock(_sessionsMutex);

	for (int32 i = 0;i < _maxSessionCount;i++) {

		shared_ptr<Session> session = make_shared<Session>(this);
		_completionPortHandler->RegisterHandle((HANDLE)session->GetSocket(), 0);

		_sessions[session->GetSessionId()] = session;

		session->Connect(_address);
	}
}
