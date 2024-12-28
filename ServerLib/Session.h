#pragma once
#include "SocketEntity.h"
#include "Service.h"
#include "User.h"

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

	// UserData
	int32 GetSessionId() { return _userInfo.GetId(); }
	UserInfo& GetUserInfo() { return _userInfo; }
	void SetSessionId(int32 id) { _userInfo.SetId(id); }
	void SetUserPosition(float x, float y, float z) { _userInfo.SetPosition(x, y, z); }
	void SetUserDirection(float x, float y, float z) { _userInfo.SetDirection(x, y, z); }

	void Process(OverlappedEvent* event, int32 numOfBytes) override;
	void CleanResource() override;

	virtual int32 OnRecv(BYTE* recvBuffer, int32 recvBytes) { return 0; }
	virtual void OnSend(int32 sendBytes) {}

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
	// TODO : Chage RAII
	RecvBuffer* _recvBuffer = nullptr;

	bool _sendRegistered = false;

private:
	mutex _mutex;

	SendEvent _sendEvent = {};
	RecvEvent _recvEvent = {};
	ConnectEvent _connectEvent = {};

private:
	UserInfo _userInfo = {};

};

class ServerSession : public Session {
public:
	ServerSession(Service* owner);
	~ServerSession();

	virtual int32 OnRecv(BYTE* recvBuffer, int32 recvBytes) override;
	virtual void OnSend(int32 sendBytes) override;

private:

};

class ClientSession : public Session {
public:
	ClientSession(Service* owner);
	~ClientSession();

	virtual int32 OnRecv(BYTE* recvBuffer, int32 recvBytes) override;
	virtual void OnSend(int32 sendBytes) override;

private:

};
