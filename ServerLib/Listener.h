#pragma once
#include "SocketEntity.h"
#include "CompletionPortHandler.h"
#include "Service.h"

class Service;

class Listener : public SocketEntity, public enable_shared_from_this<Listener>{

public :
	Listener(Service* owner);
	~Listener();

	void SetEnv(NetAddress myAddress);

	void Start(int32 acceptCount = 10);
	void ProcessAccept(AcceptEvent* acceptEvent);

	SOCKET GetSocket() { return _listenSocket; }

	void Process(OverlappedEvent* event, int32 numOfBytes) override;
	void CleanResource() override;


private:
	void RegisterAccept(AcceptEvent* acceptEvent);

	void OnAccept(shared_ptr<Session> session);

private:
	Service* _owner;
	SOCKET _listenSocket;

	NetAddress _myAddress;

private:
	vector<AcceptEvent*> _acceptEvents;

};

