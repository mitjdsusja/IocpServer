#pragma once

class Session{
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

private:
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

};

