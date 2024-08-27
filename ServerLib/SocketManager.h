#pragma once

class SocketManager{
public:
	static void Test();

	static bool SetEnv();
	static void Clear() { WSACleanup(); }

	static SOCKET CreateSocket();

	static bool Bind(SOCKET socket, NetAddress address);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	
public:
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
	static LPFN_ACCEPTEX AcceptEx;

private:
	static bool BindWindowFunction(SOCKET dummySocket, GUID guid, LPVOID* fn);

	static void HandleError(wstring msg, int32 errorCode);
};

