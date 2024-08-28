#include "pch.h"
#include <thread>

#include "CompletionPortHandler.h"

int main() {
	SocketManager::SetEnv();

	SOCKET listenSocket = SocketManager::CreateSocket();

	NetAddress address(L"127.0.0.1", 7777);
	SocketManager::Bind(listenSocket, address);
	SocketManager::Listen(listenSocket);
	cout << "Complete Set" << endl;


	SOCKET clientSocket = SocketManager::CreateSocket();
	CompletionPortHandler* completionPortHandler = new CompletionPortHandler();
	completionPortHandler->RegisterHandle((HANDLE)listenSocket, (ULONG_PTR)0);

	BYTE recvBuf[100] = {};
	AcceptEvent* acceptEvent = new AcceptEvent();
	SocketManager::Accept(listenSocket, clientSocket, recvBuf, acceptEvent);


	while (true) {
		completionPortHandler->GetCompletionEvent();
		cout << "LOOP" << endl;
	}

}