#pragma once
#include "CompletionPortHandler.h"
#include "Listener.h"

enum ServiceType {
	Server,
	Client,
};

class Service{
public:
	Service(ServiceType type, NetAddress address, int32 maxSessionCount);

	virtual void Start() abstract;

	void CompletionEventThread();

	void AddSession(shared_ptr<Session> session);
	void removeSession(shared_ptr<Session> session);
	void Broadcast(SendBuffer* sendBuffer);

	void RegisterHandle(HANDLE handle);

protected:
	set<shared_ptr<Session>> _sessions;
	CompletionPortHandler* _completionPortHandler;

	NetAddress _address;
	int32 _maxSessionCount = 0;

	ServiceType _serviceType; 

protected:
	mutex _mutex;

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

	virtual void Start() override;

private:
	
};

