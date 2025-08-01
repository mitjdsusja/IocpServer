#include "pch.h"

#include "Service.h"
#include "ThreadManager.h"
#include "BufferPool.h"
#include "PacketHandler.h"
#include "Actor.h"
#include "Job.h"

#include "messageTest.pb.h"

#include "GameSession.h"

enum {
	GQCS_THREAD_COUNT = 1,
	WORKER_THREAD_COUNT = 1,
};

int main() {
	wcout.imbue(std::locale("kor"));

	//ClientService* clientService = new ClientService(NetAddress(L"127.0.0.1", 7777), 1, []() { return make_shared<GameSession>(nullptr); });
	ClientService* clientService = new ClientService(NetAddress(L"192.168.0.14", 7777), 1, []() { return make_shared<GameSession>(nullptr); });
	PacketHandler::RegisterPacketHandlers();

	this_thread::sleep_for(1s);
	
	clientService->Start();
	// Create Thread GQCS
	for (int32 i = 0; i < GQCS_THREAD_COUNT; i++) {
		GThreadManager->Launch([=]() {
			while (true) {
				clientService->CompletionEventThread();
			}
		});
	}

	for (int32 i = 0; i < JOB_QUEUE_THREAD; i++) {
		cout << "Job Thread Start" << endl;

		GThreadManager->Launch([=]() {
			while (true) {
				AsyncJob* job = GJobQueue->Pop();
				job->Execute();
				delete job;
			}
			});
	}

	LSendBufferPool = new BufferPool();

	this_thread::sleep_for(1s);
	
	

	while (true) {

		this_thread::sleep_for(0.5s);
	}
	
	GThreadManager->Join();
}