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

	lock_guard<mutex> _lock(_sendQueueMutex);
	
	_curSessionCount++;
	cout << "Add Session : " << _curSessionCount << endl;

	session->SetSessionId(_playerId++);

	_sessions[session->GetSessionId()] = session;
}

void Service::removeSession(shared_ptr<Session> session){

	lock_guard<mutex> _lock(_sendQueueMutex);

	_curSessionCount--;
	_sessions.erase(session->GetSessionId());
	
	cout << "Current Session Count : " << _sessions.size() << endl;
}

void Service::Broadcast(shared_ptr<Buffer> sendDataBuffer){

	lock_guard<mutex> _lock(_sendQueueMutex);

	int32 sendLen = sendDataBuffer->WriteSize();
	for (const auto& [id, session] : _sessions) {
		shared_ptr<Buffer> sendBuffer = shared_ptr<Buffer>(GSendBufferPool->Pop(), [](Buffer* buffer) { GSendBufferPool->Push(buffer); });
		// TODO : copy operator
		memcpy(sendBuffer->GetBuffer(), sendDataBuffer->GetBuffer(), sendLen);
		sendBuffer->Write(sendLen);
		session->Send(sendBuffer);
	}
}

void Service::GetUserIdList(int32* array){

	int index = 0;
	for (pair<int, shared_ptr<Session>> sessionData : _sessions) {
		int32 sessionID = sessionData.second.get()->GetSessionId();
		array[index++] = sessionData.second.get()->GetSessionId();
	}
}

void Service::GetUsersInfo(vector<UserInfo*>& userInfoList){

	lock_guard<mutex> lock(_sendQueueMutex);

	for (const auto& sessionData : _sessions) {
		userInfoList.push_back(&sessionData.second->GetUserInfo());
	}
}

void Service::SetUserInfo(UserInfo srcUserInfo){

	lock_guard<mutex> lock(_sendQueueMutex);

	shared_ptr session = _sessions[srcUserInfo.GetId()];
	if (session != nullptr) {
		session->SetUserPosition(srcUserInfo.GetPosition().x, srcUserInfo.GetPosition().y, srcUserInfo.GetPosition().z);
		session->SetUserDirection(srcUserInfo.GetDirection().x, srcUserInfo.GetDirection().y, srcUserInfo.GetDirection().z);
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

void ClientService::SendMsg(shared_ptr<Buffer> sendBuffer){

	for (const auto& [id, session] : _sessions) {
		session->Send(sendBuffer);
	}
}

void ClientService::Start(){

	lock_guard<mutex> _lock(_sendQueueMutex);

	for (int32 i = 0;i < _maxSessionCount;i++) {

		shared_ptr<ServerSession> session = make_shared<ServerSession>(this);
		_completionPortHandler->RegisterHandle((HANDLE)session->GetSocket(), 0);

		_sessions.insert(pair<int, shared_ptr<Session>>(session->GetSessionId(), session));
		session->Connect(_address);
	}
}
