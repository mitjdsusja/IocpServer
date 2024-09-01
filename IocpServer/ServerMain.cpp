#include "pch.h"
#include <thread>

#include "Listener.h"
#include "CompletionPortHandler.h"

int main() {
	SocketManager::SetEnv();

	NetAddress address(L"127.0.0.1", 7777);
	Listener* listener = new Listener();
	listener->SetEnv(address);
	cout << "Complete ListenerSet" << endl;

	// Register completionPort
	CompletionPortHandler* completionPortHandler = new CompletionPortHandler();
	completionPortHandler->RegisterHandle((HANDLE)listener->GetSocket(), (ULONG_PTR)0);

	// Accept
	listener->Start(10);

	while (true) {
		completionPortHandler->GetCompletionEvent();
		cout << "LOOP" << endl;
	}

}