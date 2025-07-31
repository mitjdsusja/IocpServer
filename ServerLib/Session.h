#pragma once
#include "SocketEntity.h"

class Service;

class Session : public SocketEntity, public enable_shared_from_this<Session>{
	friend class Listener;

	enum {
		RECV_BUFFER_SIZE = 4096 * 10,
	};

public:
	Session(Service* owner);
	virtual ~Session();

	void Connect(NetAddress peerAddress);
	void Send(shared_ptr<Buffer> sendBuffer);
	void Recv();
	void Disconnect();

	SOCKET GetSocket() { return _peerSocket; }
	RecvBuffer* GetRecvBuffer() { return _recvBuffer; }
	NetAddress& GetPeerAddressRef() { return _peerAddress; }
	Service* GetOwner() { return _owner; }
	void SetOwner(Service* owner) { _owner = owner; }

	uint64 GetSessionId() { return _sessionId;}
	void SetSessionId(uint64 id) { _sessionId = id; }

	void Process(OverlappedEvent* event, int32 numOfBytes) override;
	void CleanResource() override;

	int32 OnRecv(BYTE* recvBuffer, int32 recvBytes);
	virtual void OnConnect() {};
	virtual void OnSend(int32 sendBytes) {}
	virtual void OnRecvPacket(BYTE* recvBuffer, int32 recvBytes) { ErrorHandler::HandleError(L"Session OnRecvPacket()"); };
	virtual void OnDisconnect() {};

private:
	void RegisterConnect(NetAddress& peerAddress);
	void RegisterSend();
	void RegisterRecv();

	void ProcessConnect(OverlappedEvent* event, int32 processBytes);
	void ProcessDisconnect(OverlappedEvent* event, int32 ProcessBytes);
	void ProcessSend(OverlappedEvent* event, int32 processBytes);
	void ProcessRecv(OverlappedEvent* event, int32 recvBytes);

	void SetPeerAddress(NetAddress address) { _peerAddress = address; }


private:
	Service* _owner;
	SOCKET _peerSocket;
	NetAddress _peerAddress;

	queue<shared_ptr<Buffer>> _sendQueue;
	RecvBuffer* _recvBuffer = nullptr;
	map<int32, vector<vector<BYTE>>> _recvFrames;
	map<int32, int32> _recvFrameCounts;

	atomic<bool> _isConnected = true;
	bool _sendRegistered = false;

	uint64 _sessionId = 0;

private:
	mutex _sendQueueMutex;

	SendEvent _sendEvent = {};
	RecvEvent _recvEvent = {};
	ConnectEvent _connectEvent = {};

private:
	mutex _userInfoMutex;
};
