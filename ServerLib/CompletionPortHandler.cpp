#include "pch.h"
#include "CompletionPortHandler.h"
#include "SocketEntity.h"

CompletionPortHandler::CompletionPortHandler(){

	_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}

bool CompletionPortHandler::RegisterHandle(HANDLE handle, ULONG_PTR key){

	if (NULL == CreateIoCompletionPort(handle, _completionPort, (ULONG_PTR)key, NULL)) {
		int32 err = WSAGetLastError();
		ErrorHandler::HandleError(L"RegisterHandle", err);
		return false;
	}

	return true;
}

void CompletionPortHandler::GetCompletionEvent(uint32 ms){

	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	OverlappedEvent* overlapped = nullptr;
	if (TRUE == GetQueuedCompletionStatus(_completionPort, &numOfBytes, &key, (LPOVERLAPPED*)&overlapped, ms)) {
		overlapped->_owner->Process(overlapped, numOfBytes);
	}
	else {
		int32 err = WSAGetLastError();
		if (err == WAIT_TIMEOUT) {
			//ErrorHandler::HandleError(L"GetQueuedCompletionStatus TIMEOUT", err);
		}
		else if (err == ERROR_NETNAME_DELETED) {
			overlapped->_owner->CleanResource();
		}
		else {
			spdlog::info("GetQueuedCompletionStatus : {}", err);
			//ErrorHandler::HandleError(L"GetQueuedCompletionStatus", err);
		}
	}
}
