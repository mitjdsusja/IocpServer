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
	void Send();
	void Recv();

	SOCKET GetSocket() { return _peerSocket; }
	RecvBuffer* GetRecvBuffer() { return _recvBuffer; }
	NetAddress& GetPeerAddressRef() { return _peerAddress; }

	void Process(OverlappedEvent* event, int32 numOfBytes) override;

private:
	void RegisterConnect();
	void RegisterSend(SOCKET targetSocket, SendEvent* sendEvent);
	void RegisterRecv();

	void SetPeerAddress(NetAddress address) { _peerAddress = address; }

private:
	SOCKET _peerSocket;
	NetAddress _peerAddress;

	RecvBuffer* _recvBuffer = nullptr;

private:
	SendEvent _sendEvent = {};
	RecvEvent _recvEvent = {};
	ConnectEvent _connectEvent = {};

};

