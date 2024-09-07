#include "pch.h"

#include "Listener.h"
#include "CompletionPortHandler.h"
#include "ThreadManager.h"

enum {
	GQCS_THREAD_COUNT = 5,
};

int main() {
	SocketManager::SetEnv();
	wcout.imbue(std::locale("kor"));

	NetAddress address(L"127.0.0.1", 7777);
	Listener* listener = new Listener();
	CompletionPortHandler* completionPortHandler = new CompletionPortHandler();

	listener->SetEnv(address, completionPortHandler);
	cout << "Complete ListenerSet" << endl;

	// Register completionPort
	completionPortHandler->RegisterHandle((HANDLE)listener->GetSocket(), (ULONG_PTR)0);

	// Accept
	listener->Start(10);

	
	// Create Thread GQCS
	for (int32 i = 0; i < GQCS_THREAD_COUNT; i++) {
		cout << "Thread GQCS Start" << endl;

		GThreadManager->Launch([=]() {
			while (true) {
				completionPortHandler->GetCompletionEvent();
				cout << "Server LOOP" << endl;
			}
		});
	}



	GThreadManager->Join();
}