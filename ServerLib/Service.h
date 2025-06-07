#pragma once
#include <functional>

#include "CompletionPortHandler.h"

class Listener;
class Session;

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
	void removeSession(shared_ptr<Session> session);
	void Broadcast(shared_ptr<Buffer> sendDataBuffer);

	void RegisterHandle(HANDLE handle);

	static uint64 GenerateSessionId();

public:
	int32 GetCurSessionCount() { return _curSessionCount; }

protected:
	CompletionPortHandler* _completionPortHandler;

	NetAddress _address;
	
	ServiceType _serviceType; 

protected:
	mutex _sessionsMutex = {};
	map<uint64, shared_ptr<Session>> _sessions;

	int32 _maxSessionCount = 0;
	int32 _curSessionCount = 0;
	static atomic<uint64> _sessionIdCount;

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

