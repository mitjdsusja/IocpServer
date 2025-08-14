#include "pch.h"
#include "Global.h"

#include "DummyClientService.h"
#include "ThreadManager.h"
#include "BufferPool.h"
#include "PacketHandler.h"

#include "messageTest.pb.h"

#include "GameSession.h"
#include "Player.h"
#include "Room.h"
#include "GameManager.h"

enum {
	GQCS_THREAD_COUNT = 1,
	WORKER_THREAD_COUNT = 1,
	CLIENT_COUNT = 800,
	CREATE_ROOM_COUNT = 20,
};

void RoomStressTest();
void ServerStressTest();

int main() {
	wcout.imbue(std::locale("kor"));

	this_thread::sleep_for(1s);

	DummyClientService* dummyClientService = new DummyClientService(NetAddress(L"192.168.0.14", 7777), (int32)CLIENT_COUNT, []() { return make_shared<GameSession>(nullptr); });
	PacketHandler::RegisterPacketHandlers();
	LSendBufferPool = new BufferPool();

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

		spdlog::info("ConnectedSession Count : {}", dummyClientService->GetConnectedSessionCount());

		if (CLIENT_COUNT <= dummyClientService->GetConnectedSessionCount()) {
			break;
		}
		this_thread::sleep_for(1s);
	}
	spdlog::info("Client All Connect");

	// Login 
	dummyClientService->LoginAllSession();

	while (true) {

		spdlog::info("Player Count : {}", GPlayerManager->GetPlayerCount());

		if (CLIENT_COUNT <= GPlayerManager->GetPlayerCount()) {
			break;
		}
		this_thread::sleep_for(1s);
	}
	spdlog::info("Client All Login");

	// Ping
	GThreadManager->Launch([]() {

		while (true) {
			
			GGameManager->SendPingPacket();
			this_thread::sleep_for(3s);
		}
	});

	/*------------------------------*/

	ServerStressTest();
	
	// broadcast Movement

	while (true) {

		GGameManager->PlayerMovement();
		this_thread::sleep_for(100ms);
	}

	while (true) {

		this_thread::sleep_for(0.5s);
	}
	
	GThreadManager->Join();
}

void RoomStressTest() {

	// Request Room List
	vector<RoomInfo> roomList;
	while (true) {
		GPlayerManager->RequestRoomList();
		this_thread::sleep_for(1s);

		roomList = GGameManager->GetEnterableRoomList();
		if (roomList.size() == 0) {

			spdlog::info("ROOM LIST EMPTY");
		}
		else {
			break;
		}
	}

	// Enter Room
	GGameManager->RequestEnterRoomAllPlayer(roomList[0]._roomId);
	GGameManager->SetEnterRoomId(roomList[0]._roomId);
	while (true) {

		spdlog::info("Enter Room Count : {}", GGameManager->GetEnteredPlayerCount());

		if (CLIENT_COUNT <= GGameManager->GetEnteredPlayerCount()) {
			break;
		}
		this_thread::sleep_for(1s);
	}
	spdlog::info("Client All Enter Room");
}
void ServerStressTest() {

	// Create Room
	GGameManager->ReqeustCreateRoom(CREATE_ROOM_COUNT);
	while (true) {

		if (GGameManager->_createRoomPlayerSessionCount.load() == CREATE_ROOM_COUNT) {

			spdlog::info("CREATE ALL ROOM");
			break;
		}
		this_thread::sleep_for(1s);
	}

	// Request Room List
	vector<RoomInfo> roomList;
	while (true) {
		GPlayerManager->RequestRoomList();
		this_thread::sleep_for(1s);

		roomList = GGameManager->GetEnterableRoomList();
		if (roomList.size() == 0) {

			spdlog::info("ROOM LIST EMPTY");
		}
		else {
			break;
		}
	}

	for (auto& room : roomList) {
		spdlog::info("Room id : {}", room._roomId);
	}

	// Enter Room
	GGameManager->ReqeustEnterRoomAllPlayer();
	while (true) {

		spdlog::info("Enter Room Count : {}", GGameManager->GetEnteredPlayerCount());

		if (CLIENT_COUNT <= GGameManager->GetEnteredPlayerCount()) {
			break;
		}
		this_thread::sleep_for(1s);
	}
	spdlog::info("Client All Enter Room");

}