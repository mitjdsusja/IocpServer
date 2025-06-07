#include "pch.h"
#include "SocketManager.h"
#include "SocketEntity.h"

LPFN_CONNECTEX SocketManager::ConnectEx = nullptr;
LPFN_DISCONNECTEX SocketManager::DisconnectEx = nullptr;
LPFN_ACCEPTEX SocketManager::AcceptEx = nullptr;


bool SocketManager::SetEnv(){

	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0) {
		ErrorHandler::HandleError(L"WSAStartup Error", err);

		return false;
	}

	SOCKET dummySocket = CreateSocket();
	bool flag = true;
	flag = BindWindowFunction(dummySocket, WSAID_CONNECTEX, (LPVOID*)&ConnectEx);
	flag = BindWindowFunction(dummySocket, WSAID_DISCONNECTEX, (LPVOID*)&DisconnectEx);
	flag = BindWindowFunction(dummySocket, WSAID_ACCEPTEX, (LPVOID*)&AcceptEx);
	closesocket(dummySocket);

	return flag;
}

SOCKET SocketManager::CreateSocket(){

	SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (socket == INVALID_SOCKET) {
		int32 err = WSAGetLastError();
		ErrorHandler::HandleError(L"CreateSocket Error", err);
		
	}
	return socket;
}

bool SocketManager::Bind(SOCKET socket, NetAddress address) {

	if (SOCKET_ERROR == ::bind(socket, (SOCKADDR*)&address.GetSockAddr(), sizeof(SOCKADDR))) {
		int32 err = WSAGetLastError();
		ErrorHandler::HandleError(L"Bind Error", err);

		return false;
	}

	return true;
}

bool SocketManager::BindAnyAddress(SOCKET socket, uint16 port) {

	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == ::bind(socket, (SOCKADDR*)&address, sizeof(SOCKADDR_IN))) {
		int32 err = WSAGetLastError();
		ErrorHandler::HandleError(L"BindAnyAddress Error", err);

		return false;
	}

	return true;
}

bool SocketManager::Listen(SOCKET socket, int32 backlog) {

	if (SOCKET_ERROR == listen(socket, backlog)) {
		int32 err = WSAGetLastError();
		ErrorHandler::HandleError(L"Listen Error", err);

		return false;
	}

	return true;
}

bool SocketManager::Send(SOCKET targetSocket, Buffer* sendBufferArray, int32 bufCount, SendEvent* sendEvent){
	
	if (targetSocket == INVALID_SOCKET) {
		ErrorHandler::HandleError(L"RECV INVALID SOCKET");
		return false;
	}

	vector<WSABUF> wsaBufs;
	wsaBufs.resize(bufCount);
	for (int32 i = 0;i < bufCount;i++) {
		wsaBufs[i].buf = (char*)sendBufferArray->GetBuffer();
		wsaBufs[i].len = sendBufferArray->WriteSize();
	}

	DWORD bytes = 0;
	if (SOCKET_ERROR == ::WSASend(targetSocket, wsaBufs.data(), (DWORD)bufCount, &bytes, 0, sendEvent, nullptr)) {
		int32 err = WSAGetLastError();
		if (err == WSA_IO_PENDING) {
			// TODO : PENDING
			return true;
		}
		else {
			// TODO : Error
			ErrorHandler::HandleError(L"Recv Failed", err);
			return false;
		}
	}

	return true;
}

bool SocketManager::Recv(SOCKET targetSocket, RecvBuffer* recvBuffer, RecvEvent* recvEvent){

	if (targetSocket == INVALID_SOCKET) {
		ErrorHandler::HandleError(L"RECV INVALID SOCKET");
		return false;
	}

	DWORD recvBytes = 0;
	DWORD flag = 0;
	WSABUF wsaBuf;
	wsaBuf.buf = (char*)recvBuffer->WritePos();
	wsaBuf.len = recvBuffer->FreeSize();

	if (SOCKET_ERROR == WSARecv(targetSocket, &wsaBuf, 1, &recvBytes, &flag, recvEvent, NULL)) {
		int32 err = WSAGetLastError();
		if (err == WSA_IO_PENDING) {
			// TODO : PENDING
			return true;
		}
		else if (err == WSAECONNRESET) {
			recvEvent->_owner->CleanResource();
		}
		else {
			// TODO : Error
			ErrorHandler::HandleError(L"Recv Failed", err);
			return false;
		}
	}

	return true;
}

bool SocketManager::Accept(SOCKET listenSocket, SOCKET AcceptSocket,BYTE* recvBuf, AcceptEvent* acceptEvent){
	
	DWORD bytes = 0;
	if (false == AcceptEx(listenSocket, AcceptSocket, recvBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPOVERLAPPED)acceptEvent)) {
		int32 err = WSAGetLastError();

		if (err == WSA_IO_PENDING) {
			return true;
		}
		else {
			ErrorHandler::HandleError(L"AcceptEx Error", err);

			return false;
		}
	}

	return true;
}

bool SocketManager::Connect(SOCKET targetSocket, SOCKADDR* targetAddr, ConnectEvent* connectEvent){

	DWORD bytes = 0;
	if (false == ConnectEx(targetSocket, targetAddr, sizeof(SOCKADDR), nullptr, 0, &bytes, connectEvent)) {
		int32 err = WSAGetLastError();
		if (err == WSA_IO_PENDING) {
			return true;
		}
		else {
			ErrorHandler::HandleError(L"Connect", err);

			return false;
		}
	}

	return true;
}

bool SocketManager::BindWindowFunction(SOCKET dummySocket, GUID guid, LPVOID* fn){

	DWORD bytes = 0;
	if (SOCKET_ERROR == ::WSAIoctl(dummySocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), &bytes, NULL, NULL)) {
		int32 err = WSAGetLastError();
		ErrorHandler::HandleError(L"WSAStartup Error", err);

		return false;
	}

	return true;
}
