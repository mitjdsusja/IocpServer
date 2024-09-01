#pragma once

class Session{
	enum {
		RECV_BUFFER_SIZE = 4096,
	};
public:
	Session(NetAddress address);
	~Session();

	void Accept();
	void Connect();
	void Send();
	void Recv();

public:
	void RegisterAccept();
	void RegisterConnect();
	void RegisterSend();
	void RegisterRecv();

private:
	SOCKET _peerSocket;
	NetAddress _address;

	RecvBuffer* _recvBuffer = nullptr;

private:
	SendEvent _sendEvent = {};
	RecvEvent _recvEvent = {};
	ConnectEvent _connectEvent = {};
	AcceptEvent _acceptEvent = {};
};

