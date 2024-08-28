#include "pch.h"

int main() {
	SocketManager::SetEnv();

	SOCKET listenSocket = SocketManager::CreateSocket();

	NetAddress address(L"127.0.0.1", 7777);
	SocketManager::Bind(listenSocket, address);
	SocketManager::Listen(listenSocket);
	cout << "Complete Set" << endl;

	SOCKET clientSocket = SocketManager::CreateSocket();
	SocketManager::RegisterHandle((HANDLE)clientSocket, (ULONG_PTR)0);

	BYTE recvBuf[100] = {};
	OVERLAPPED overlapped = {};
	SocketManager::Accept(listenSocket, clientSocket, recvBuf, &overlapped);

	//GetQueuedCompletionStatus()

	while (true) {
		

	}
}