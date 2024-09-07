#include "pch.h"

#include "CompletionPortHandler.h"
#include "ThreadManager.h"
#include "BufferPool.h"

enum {
	GQCS_THREAD_COUNT = 5,
};

int main() {
	SocketManager::SetEnv();

	Session* serverSession = new Session();

	NetAddress serverAddr(L"127.0.0.1", 7777);

	CompletionPortHandler* completionPortHandler = new CompletionPortHandler();
	completionPortHandler->RegisterHandle((HANDLE)serverSession->GetSocket(), (ULONG_PTR)0);

	this_thread::sleep_for(1s);
	serverSession->Connect(serverAddr);

	// Create Thread GQCS
	for (int32 i = 0; i < GQCS_THREAD_COUNT; i++) {
		GThreadManager->Launch([=]() {
			while (true) {
				completionPortHandler->GetCompletionEvent();
				cout << "Server LOOP" << endl;
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

		serverSession->Send(sendBuffer);

		GSendBufferPool->Push(sendBuffer);
		this_thread::sleep_for(0.1s);
	}
	
	
	GThreadManager->Join();
}