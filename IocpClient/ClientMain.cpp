#include "pch.h"

#include "Service.h"
#include "ThreadManager.h"
#include "BufferPool.h"
#include "PacketHandler.h"

enum {
	GQCS_THREAD_COUNT = 5,
};

int main() {
	wcout.imbue(std::locale("kor"));

	//ClientService* clientService = new ClientService(NetAddress(L"127.0.0.1", 7777), 1);
	ClientService* clientService = new ClientService(NetAddress(L"192.168.0.14", 7777), 1);

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

	
	// Get User Info
	{
		SendBuffer* sendBuffer = GSendBufferPool->Pop();
		Packet_C_Request_User_Info* packet = (Packet_C_Request_User_Info*)sendBuffer->Buffer();

		packet->packetId = PKT_C_REQUEST_USER_INFO;
		packet->packetSize = sizeof(Packet_C_Request_User_Info);

		sendBuffer->Write(packet->packetSize);

		clientService->SendMsg(sendBuffer);
	}

	// Get Other User Info
	{
		SendBuffer* sendBuffer = GSendBufferPool->Pop();
		Packet_C_Request_Other_User_Info* packet = (Packet_C_Request_Other_User_Info*)sendBuffer->Buffer();

		packet->packetId = PKT_C_REQUEST_OTHER_USER_INFO;
		packet->packetSize = sizeof(Packet_C_Request_Other_User_Info);

		sendBuffer->Write(packet->packetSize);

		clientService->SendMsg(sendBuffer);
	}

	while (true) {

		this_thread::sleep_for(0.5s);
	}
	
	
	GThreadManager->Join();
}