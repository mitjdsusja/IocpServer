#pragma once

class Listener{
public:

public :
	Listener();
	~Listener();

	bool SetEnv(NetAddress myAddress);

	void Start(int32 acceptCount);
	void RegisterAccept(AcceptEvent* acceptEvent);

	SOCKET GetSocket() { return _listenSocket; }

private:
	SOCKET _listenSocket;

	NetAddress _myAddress;

private:
	vector<AcceptEvent*> _acceptEvents;

};

