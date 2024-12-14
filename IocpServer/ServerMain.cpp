#include "pch.h"

#include <windows.h>

#include "ThreadManager.h"

#include "Service.h"
#include "PacketHandler.h"
#include "JobQueue.h"

enum {
	GQCS_THREAD_COUNT = 5,
};

int main() {
	wcout.imbue(std::locale("kor"));

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	
	ServerService* serverService = new ServerService(NetAddress(L"127.0.0.1", 7777), 100);
	//ServerService* serverService = new ServerService(NetAddress(L"192.168.0.14", 7777), 10);
	PacketHandler::Init();

	serverService->Start();
	// Create Thread GQCS
	for (int32 i = 0; i < (int32)sysInfo.dwNumberOfProcessors; i++) {
		cout << "GQCS Thread Start" << endl;

		GThreadManager->Launch([=]() {
			while (true) {
				serverService->CompletionEventThread();
			}
		});
	}

	for (int32 i = 0; i < (int32)sysInfo.dwNumberOfProcessors; i++) {
		cout << "Job Thread Start" << endl;

		GThreadManager->Launch([=]() {
			while (true) {
				auto job = GJobQueue->Pop();
				job();
			}
		});
	}

	GThreadManager->Join();
}