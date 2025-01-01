#pragma once
#include "CompletionPortHandler.h"
#include "Listener.h"
#include "User.h"

enum ServiceType {
	Server,
	Client,
};

class Service{
public:
	Service(ServiceType type, NetAddress address, int32 maxSessionCount);
	virtual ~Service();

	virtual void Start() abstract;

	void CompletionEventThread(uint32 ms = INFINITE);

	void AddSession(shared_ptr<Session> session);
	void removeSession(shared_ptr<Session> session);
	void Broadcast(shared_ptr<Buffer> sendDataBuffer);

	// Users
	int32 GetCurSessionCount() { return _curSessionCount; }
	void GetUsersInfo(vector<UserInfo>& userInfoList);

	void SetUserInfo(UserInfo srcUserInfo);
	
	void RegisterHandle(HANDLE handle);

protected:
	CompletionPortHandler* _completionPortHandler;

	NetAddress _address;
	
	ServiceType _serviceType; 

protected:
	mutex _sessionsMutex;
	map<int, shared_ptr<Session>> _sessions;

	int32 _maxSessionCount = 0;
	int32 _curSessionCount = 0;
	int32 _playerId = 1;

};

class ServerService : public Service {
public:
	ServerService(NetAddress addres, int32 maxSessionCount);

	virtual void Start() override;

private:
	shared_ptr<Listener> _listener;

};

class ClientService : public Service {
public:
	ClientService(NetAddress address, int32 maxSessionCount);

	void SendMsg(shared_ptr<Buffer> sendBuffer);

	virtual void Start() override;

private:
	
};

