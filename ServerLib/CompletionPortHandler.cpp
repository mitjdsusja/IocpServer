#include "pch.h"
#include "CompletionPortHandler.h"

CompletionPortHandler::CompletionPortHandler(){

	_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}

bool CompletionPortHandler::RegisterHandle(HANDLE handle, ULONG_PTR key){

	if (NULL == CreateIoCompletionPort(handle, _completionPort, (ULONG_PTR)0, NULL)) {
		int32 err = WSAGetLastError();
		ErrorHandler::HandleError(L"RegisterHandle", err);
	}

	return false;
}

void CompletionPortHandler::GetCompletionEvent(){
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	OverlappedEvent* overlapped = nullptr;
	if (true == GetQueuedCompletionStatus(_completionPort, &numOfBytes, &key, (LPOVERLAPPED*)&overlapped, INFINITE)) {
		switch (overlapped->_eventType) {
			case EventType::ACCEPT: 
				cout << "[ACCEPT] " << endl;
				break;
			case EventType::CONNECT:
				cout << "[CONNECT] " << endl;
				break;
			case EventType::DISCONNECT:
				cout << "[DISCONNECT] " << endl;
				break;
			case EventType::SEND:
				cout << "[SEND] " << endl;
				break;
			case EventType::RECV:
				cout << "[RECV] " << endl;
				break;
			default:
				cout << "[DEFAULT] " << endl;
				break;
		}
	}
	else {
		int32 err = WSAGetLastError();
		if (err == WAIT_TIMEOUT) {
			ErrorHandler::HandleError(L"GetQueuedCompletionStatus TIMEOUT", err);
		}
		else {
			ErrorHandler::HandleError(L"GetQueuedCompletionStatus", err);
		}
	}
}
