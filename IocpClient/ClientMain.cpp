#include "pch.h"

int main() {
	SocketManager::SetEnv();

	SOCKET serverSocket = SocketManager::CreateSocket();

	NetAddress serverAddr(L"127.0.0.1", 7777);

	DWORD bytes = 0;
	OVERLAPPED overlapped = {};
	SocketManager::ConnectEx(serverSocket, (SOCKADDR*)&serverAddr.GetSockAddr(), sizeof(SOCKADDR), nullptr, 0, &bytes, &overlapped);



}