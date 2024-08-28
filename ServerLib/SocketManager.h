#pragma once

class SocketManager{
public:
	static bool SetEnv();
	static void Clear() { WSACleanup(); }

	static SOCKET CreateSocket();
	static bool RegisterHandle(HANDLE handle, ULONG_PTR key);

	static bool Bind(SOCKET socket, NetAddress address);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static bool Send();
	static bool Recv();
	static bool Accept(SOCKET listenSocket, SOCKET AcceptSocket, BYTE* recvBuf, OVERLAPPED* overlapped);
	
public:
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
	static LPFN_ACCEPTEX AcceptEx;

private:
	static bool BindWindowFunction(SOCKET dummySocket, GUID guid, LPVOID* fn);

	static void HandleError(wstring msg, int32 errorCode);

private:
	static HANDLE completionPort;
};

