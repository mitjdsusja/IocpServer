#include "pch.h"

#include "Listener.h"
#include "CompletionPortHandler.h"
#include "ThreadManager.h"

enum {
	GQCS_THREAD_COUNT = 5,
};

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

	
	// Create Thread GQCS
	for (int32 i = 0; i < GQCS_THREAD_COUNT; i++) {
		GThreadManager->Launch([=]() {
			completionPortHandler->GetCompletionEvent();
			cout << "CLIENT LOOP" << endl;
			});
	}



	GThreadManager->Join();
}