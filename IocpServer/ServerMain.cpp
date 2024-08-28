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
	AcceptEvent* acceptEvent = new AcceptEvent();
	SocketManager::Accept(listenSocket, clientSocket, recvBuf, acceptEvent);

	DWORD bytes[100] = {};
	ULONG_PTR key = 0;
	OverlappedEvent* overlapped;

	while (true) {
		if (true == GetQueuedCompletionStatus(SocketManager::CompletionPort(), bytes, &key, (LPOVERLAPPED*)&overlapped, INFINITE)) {
			switch (overlapped->_eventType) {
				case EventType::ACCEPT:
					cout << "[ACCEPT] " << endl;
					break;
				case EventType::CONNECT:
					cout << "[CONNECT] " << endl;
					break;
				case EventType::DISCONNECT:
					cout << "[DISCONNECT] " << endl;
					break;
				case EventType::SEND:
					cout << "[SEND] " << endl;
					break;
				case EventType::RECV:
					cout << "[RECV] " << endl;
					break;
				default:
					cout << "[DEFAULT] " << endl;
					break;
			}
		}
		else {
			int32 err = WSAGetLastError();
			if (err == WAIT_TIMEOUT) {
				cout << "TIMEOUT" << endl;
			}
			else {
				this_thread::sleep_for(1s);
				cout << "[ERROR] :  " << err << endl;
			}
		}

	}
}