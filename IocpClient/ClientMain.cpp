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
	
	int32 id = 1;
	while (true) {
		SendBuffer* sendBuffer = GSendBufferPool->Pop();
		Packet_C_Request_Info* packet = (Packet_C_Request_Info*)sendBuffer->Buffer();
		
		packet->packetId = PKT_C_REQUEST_INFO;

		sendBuffer->Write(sizeof(PKT_C_REQUEST_INFO));

		packet->packetSize = sendBuffer->WriteSize();

		clientService->SendMsg(sendBuffer);

		GSendBufferPool->Push(sendBuffer);
		this_thread::sleep_for(0.5s);
	}
	
	
	GThreadManager->Join();
}