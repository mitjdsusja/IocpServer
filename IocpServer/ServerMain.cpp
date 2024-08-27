#include "pch.h"

int main() {
	SocketManager::SetEnv();

	SOCKET listenSocket = SocketManager::CreateSocket();

	NetAddress address(L"127.0.0.1", 7777);
	SocketManager::Bind(listenSocket, address);
	SocketManager::Listen(listenSocket);
	cout << "Complete" << endl;
	while (true) {


	}
}