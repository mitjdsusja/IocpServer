#pragma once
#include "SocketEntity.h"

class Listener : public SocketEntity{

public :
	Listener();
	~Listener();

	bool SetEnv(NetAddress myAddress);

	void Start(int32 acceptCount);
	void ProcessAccept(AcceptEvent* acceptEvent);

	SOCKET GetSocket() { return _listenSocket; }

	void Process(OverlappedEvent* event, int32 numOfBytes) override;

private:
	void RegisterAccept(AcceptEvent* acceptEvent);

	void OnAccept(Session* session);

private:
	SOCKET _listenSocket;

	NetAddress _myAddress;

private:
	vector<AcceptEvent*> _acceptEvents;

};

