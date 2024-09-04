#pragma once
#include "SocketEntity.h"

class Session : public SocketEntity{
	friend class Listener;

	enum {
		RECV_BUFFER_SIZE = 4096,
	};

public:
	Session();
	~Session();

	void Connect(NetAddress peerAddress);
	void Send(SendBuffer* sendBuffer);
	void Recv();

	SOCKET GetSocket() { return _peerSocket; }
	RecvBuffer* GetRecvBuffer() { return _recvBuffer; }
	NetAddress& GetPeerAddressRef() { return _peerAddress; }

	void Process(OverlappedEvent* event, int32 numOfBytes) override;

private:
	void RegisterConnect(NetAddress& peerAddress);
	void RegisterSend();
	void RegisterRecv();

	void ProcessConnect();
	void ProcessSend();
	void ProcessRecv();

	void SetPeerAddress(NetAddress address) { _peerAddress = address; }

private:
	SOCKET _peerSocket;
	NetAddress _peerAddress;

	queue<SendBuffer*> sendQueue;
	RecvBuffer* _recvBuffer = nullptr;

	bool sendRegistered = false;

private:
	mutex _mutex;

	SendEvent _sendEvent = {};
	RecvEvent _recvEvent = {};
	ConnectEvent _connectEvent = {};

};

