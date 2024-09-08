#include "pch.h"

#include "Service.h"
#include "ThreadManager.h"
#include "BufferPool.h"

enum {
	GQCS_THREAD_COUNT = 5,
};

int main() {
	wcout.imbue(std::locale("kor"));

	ClientService* clientService = new ClientService(NetAddress(L"127.0.0.1", 7777), 1);

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



	///////////////////////////////////////////

	/*SendBuffer* sendBuffer = GSendBufferPool->Pop();
	BYTE* buffer = sendBuffer->Buffer();

	wstring msg(L"Send Message 첫 메세지 입니다. !!@@");
	int32 len = (msg.size() + 1) * sizeof(WCHAR);

	memcpy(buffer, msg.c_str(), len);
	sendBuffer->Write(len);

	serverSession->Send(sendBuffer);

	GSendBufferPool->Push(sendBuffer);
	this_thread::sleep_for(0.5s);*/
	
	while (true) {
		SendBuffer* sendBuffer = GSendBufferPool->Pop();
		BYTE* buffer = sendBuffer->Buffer();

		wstring msg(L"Send Message 첫 메세지 입니다. !!@@");
		int32 len = (msg.size() + 1) * sizeof(WCHAR);

		memcpy(buffer, msg.c_str(), len);
		sendBuffer->Write(len);

		clientService->SendMsg(sendBuffer);

		GSendBufferPool->Push(sendBuffer);
		this_thread::sleep_for(0.1s);
	}
	
	
	GThreadManager->Join();
}