#include "pch.h"

#include "ThreadManager.h"

#include "Service.h"
#include "PacketHandler.h"



enum {
	GQCS_THREAD_COUNT = 5,
};

int main() {
	wcout.imbue(std::locale("kor"));
	
	//ServerService* serverService = new ServerService(NetAddress(L"127.0.0.1", 7777), 100);
	ServerService* serverService = new ServerService(NetAddress(L"192.168.0.14", 7777), 10);
	PacketHandler::Init();

	serverService->Start();
	// Create Thread GQCS
	for (int32 i = 0; i < GQCS_THREAD_COUNT; i++) {
		cout << "Thread GQCS Start" << endl;

		GThreadManager->Launch([=]() {
			while (true) {
				serverService->CompletionEventThread();
			}
		});
	}



	GThreadManager->Join();
}