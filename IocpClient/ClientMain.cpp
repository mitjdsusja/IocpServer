#include "pch.h"

#include "Service.h"
#include "ThreadManager.h"
#include "BufferPool.h"
#include "PacketHandler.h"

#include "messageTest.pb.h"

enum {
	GQCS_THREAD_COUNT = 5,
};

int main() {
	wcout.imbue(std::locale("kor"));

	ClientService* clientService = new ClientService(NetAddress(L"127.0.0.1", 7777), 1);
	//ClientService* clientService = new ClientService(NetAddress(L"192.168.0.14", 7777), 1);

	this_thread::sleep_for(1s);
	
	PacketHandler::Init();

	clientService->Start();
	// Create Thread GQCS
	for (int32 i = 0; i < GQCS_THREAD_COUNT; i++) {
		GThreadManager->Launch([=]() {
			while (true) {
				clientService->CompletionEventThread();
			}
		});
	}

	LSendBufferPool = new BufferPool();

	this_thread::sleep_for(1s);
	
	// Get User Info
	{
		msgTest::CS_Request_User_Info packetRequestUserInfo;
		Buffer* sendBuffer = PacketHandler::MakeSendBuffer(packetRequestUserInfo, PacketId::PKT_CS_REQUEST_USER_INFO);

		clientService->SendMsg(sendBuffer);
	}

	//Get Other User Info
	{
		msgTest::CS_Request_Other_User_Info packetOtherUserInfo;

		Buffer* sendBuffer = PacketHandler::MakeSendBuffer(packetOtherUserInfo, PacketId::PKT_CS_REQUEST_OTHER_USER_INFO);
		clientService->SendMsg(sendBuffer);
	}

	while (true) {

		this_thread::sleep_for(0.5s);
	}
	
	GThreadManager->Join();
}