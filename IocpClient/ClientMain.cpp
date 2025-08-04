#include "pch.h"
#include "Global.h"

#include "Service.h"
#include "ThreadManager.h"
#include "BufferPool.h"
#include "PacketHandler.h"

#include "messageTest.pb.h"

#include "GameSession.h"
#include "Player.h"

enum {
	GQCS_THREAD_COUNT = 1,
	WORKER_THREAD_COUNT = 1,
	CLIENT_COUNT = 10,
};

int main() {
	wcout.imbue(std::locale("kor"));

	//ClientService* clientService = new ClientService(NetAddress(L"127.0.0.1", 7777), 1, []() { return make_shared<GameSession>(nullptr); });
	ClientService* clientService = new ClientService(NetAddress(L"192.168.0.14", 7777), (int32)CLIENT_COUNT, []() { return make_shared<GameSession>(nullptr); });
	PacketHandler::RegisterPacketHandlers();
	LSendBufferPool = new BufferPool();

	//this_thread::sleep_for(1s);
	
	clientService->Start();
	// Create Thread GQCS
	for (int32 i = 0; i < GQCS_THREAD_COUNT; i++) {
		GThreadManager->Launch([=]() {
			while (true) {
				clientService->CompletionEventThread();
			}
		});
	}

	// wait all connect
	while (true) {

		wcout << L"session Count : " << clientService->GetCurSessionCount() << endl;
		wcout << L"player Count : " << GPlayerManager->GetPlayerCount() << endl;

		if (CLIENT_COUNT <= clientService->GetCurSessionCount() && CLIENT_COUNT <= GPlayerManager->GetPlayerCount()) {
			break;
		}
		this_thread::sleep_for(1s);
	}
	wcout << L"Client All Connect" << endl;

	// Login 
	// session을 통해 send 후 handle후 sessionId를 받아서 Player객체 생성
	//

	// Request Room List

	// Enter Room

	// broadcast Movement
	
	

	while (true) {

		this_thread::sleep_for(0.5s);
	}
	
	GThreadManager->Join();
}