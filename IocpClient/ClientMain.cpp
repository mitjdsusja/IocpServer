#include "pch.h"

int main() {
	SOCKET listenSocket = SocketManager::CreateSocket();
	SocketManager::SetEnv();
	SocketManager::BindAnyAddress(listenSocket, 7777);

}