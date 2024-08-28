#include "pch.h"
#include <thread>

#include "CompletionPortHandler.h"

int main() {
	SocketManager::SetEnv();

	SOCKET serverSocket = SocketManager::CreateSocket();

	NetAddress serverAddr(L"127.0.0.1", 7777);
	SocketManager::BindAnyAddress(serverSocket, 0);
	CompletionPortHandler* completionPortHandler = new CompletionPortHandler();
	completionPortHandler->RegisterHandle((HANDLE)serverSocket, (ULONG_PTR)0);

	DWORD bytes = 0;
	ConnectEvent* connectEvent = new ConnectEvent();

	this_thread::sleep_for(1s);
	SocketManager::Connect(serverSocket, (SOCKADDR*)&serverAddr.GetSockAddr(), connectEvent);

	while (true) {
		
		completionPortHandler->GetCompletionEvent();
		cout << "LOOP" << endl;
	}

}