#pragma once
#include "OverlappedEvent.h"

class SocketManager{
public:
	static bool SetEnv();
	static void Clear() { WSACleanup(); }

	static SOCKET CreateSocket();
	

	static bool Bind(SOCKET socket, NetAddress address);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static bool Send();
	static bool Recv();
	static bool Accept(SOCKET listenSocket, SOCKET AcceptSocket, BYTE* recvBuf, AcceptEvent* acceptEvent);
	static bool Connect(SOCKET targetSocket, SOCKADDR* targetAddr, ConnectEvent* connectEvent);

private:
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
	static LPFN_ACCEPTEX AcceptEx;

private:
	static bool BindWindowFunction(SOCKET dummySocket, GUID guid, LPVOID* fn);

private:

};

