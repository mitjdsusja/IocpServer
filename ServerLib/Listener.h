#pragma once
#include "SocketEntity.h"
#include "CompletionPortHandler.h"

class Listener : public SocketEntity{

public :
	Listener();
	~Listener();

	bool SetEnv(NetAddress myAddress, CompletionPortHandler* cpHandler);

	void Start(int32 acceptCount);
	void ProcessAccept(AcceptEvent* acceptEvent);

	SOCKET GetSocket() { return _listenSocket; }

	void Process(OverlappedEvent* event, int32 numOfBytes) override;

private:
	void RegisterAccept(AcceptEvent* acceptEvent);
	void RegisterSocket(SOCKET socket);

	void OnAccept(Session* session);

private:
	SOCKET _listenSocket;
	CompletionPortHandler* _completionPortHandler;

	NetAddress _myAddress;

private:
	vector<AcceptEvent*> _acceptEvents;

};

