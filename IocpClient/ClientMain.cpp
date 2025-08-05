#include "pch.h"
#include "Global.h"

#include "DummyClientService.h"
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
	DummyClientService* dummyClientService = new DummyClientService(NetAddress(L"192.168.0.14", 7777), (int32)CLIENT_COUNT, []() { return make_shared<GameSession>(nullptr); });
	PacketHandler::RegisterPacketHandlers();
	LSendBufferPool = new BufferPool();

	//this_thread::sleep_for(1s);
	
	dummyClientService->Start();
	// Create Thread GQCS
	for (int32 i = 0; i < GQCS_THREAD_COUNT; i++) {
		GThreadManager->Launch([=]() {
			while (true) {
				dummyClientService->CompletionEventThread();
			}
		});
	}

	// wait all connect
	while (true) {

		wcout << L"ConnectedSession Count : " << dummyClientService->GetConnectedSessionCount() << endl;

		if (CLIENT_COUNT <= dummyClientService->GetConnectedSessionCount()) {
			break;
		}
		this_thread::sleep_for(1s);
	}
	wcout << L"Client All Connect" << endl;

	// Login 
	dummyClientService->LoginAllSession();

	while (true) {

		wcout << L"Player Count : " << GPlayerManager->GetPlayerCount() << endl;

		if (CLIENT_COUNT <= GPlayerManager->GetPlayerCount()) {
			break;
		}
		this_thread::sleep_for(1s);
	}
	wcout << L"Client All Login" << endl;

	// Request Room List
	GPlayerManager->RequestRoomList();

	// Enter Room

	// broadcast Movement
	
	

	while (true) {

		this_thread::sleep_for(0.5s);
	}
	
	GThreadManager->Join();
}