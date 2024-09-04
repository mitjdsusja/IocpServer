#include "pch.h"

#include "CompletionPortHandler.h"
#include "ThreadManager.h"

enum {
	GQCS_THREAD_COUNT = 5,
};

int main() {
	SocketManager::SetEnv();

	Session* serverSession = new Session();

	NetAddress serverAddr(L"127.0.0.1", 7777);

	CompletionPortHandler* completionPortHandler = new CompletionPortHandler();
	completionPortHandler->RegisterHandle((HANDLE)serverSession->GetSocket(), (ULONG_PTR)0);

	this_thread::sleep_for(1s);
	serverSession->Connect(serverAddr);

	// Create Thread GQCS
	for (int32 i = 0; i < GQCS_THREAD_COUNT; i++) {
		GThreadManager->Launch([=]() {
				completionPortHandler->GetCompletionEvent();
				cout << "CLIENT LOOP" << endl;
		});
	}

	GThreadManager->Join();
}