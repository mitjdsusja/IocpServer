#pragma once
#include <functional>

#include "CompletionPortHandler.h"
#include "Listener.h"
#include "Session.h"
#include "User.h"

enum ServiceType {
	Server,
	Client,
};

class Service{
public:
	Service(ServiceType type, NetAddress address, int32 maxSessionCount, function<shared_ptr<Session>(void)> sessionCreateFunc);
	virtual ~Service();

	virtual void Start() abstract;

	void CompletionEventThread(uint32 ms = INFINITE);

	shared_ptr<Session> CreateSession();
	void AddSession(shared_ptr<Session> session);
	uint64 GenerateSessionId();
	void removeSession(shared_ptr<Session> session);
	void Broadcast(shared_ptr<Buffer> sendDataBuffer);

	void RegisterHandle(HANDLE handle);

public:
	int32 GetCurSessionCount() { return _curSessionCount; }
	void GetUsersInfo(vector<UserInfo>& userInfoList);

	void SetUserInfo(UserInfo srcUserInfo);

protected:
	CompletionPortHandler* _completionPortHandler;

	NetAddress _address;
	
	ServiceType _serviceType; 

protected:
	mutex _sessionsMutex = {};
	map<uint64, shared_ptr<Session>> _sessions;

	int32 _maxSessionCount = 0;
	int32 _curSessionCount = 0;

	function<shared_ptr<Session>(void)> _sessionCreateFunc;
};

class ServerService : public Service {
public:
	ServerService(NetAddress addres, int32 maxSessionCount, function<shared_ptr<Session>(void)> sessionCreateFunc);

	virtual void Start() override;

private:
	shared_ptr<Listener> _listener;

};

class ClientService : public Service {
public:
	ClientService(NetAddress address, int32 maxSessionCount, function<shared_ptr<Session>(void)> sessionCreateFunc);

	void SendMsg(shared_ptr<Buffer> sendBuffer);

	virtual void Start() override;

private:
	
};

