#include "pch.h"
#include "SocketManager.h"

LPFN_CONNECTEX SocketManager::ConnectEx = nullptr;
LPFN_DISCONNECTEX SocketManager::DisconnectEx = nullptr;
LPFN_ACCEPTEX SocketManager::AcceptEx = nullptr;

HANDLE SocketManager::completionPort = nullptr;

bool SocketManager::SetEnv(){

	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0) {
		HandleError(L"WSAStartup Error", err);

		return false;
	}

	SOCKET dummySocket = CreateSocket();
	bool flag = true;
	flag = BindWindowFunction(dummySocket, WSAID_CONNECTEX, (LPVOID*)&ConnectEx);
	flag = BindWindowFunction(dummySocket, WSAID_DISCONNECTEX, (LPVOID*)&DisconnectEx);
	flag = BindWindowFunction(dummySocket, WSAID_ACCEPTEX, (LPVOID*)&AcceptEx);
	closesocket(dummySocket);

	HANDLE completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	return flag;
}

SOCKET SocketManager::CreateSocket(){

	SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (socket == INVALID_SOCKET) {
		int32 err = WSAGetLastError();
		HandleError(L"CreateSocket Error", err);
		
	}
	return socket;
}

bool SocketManager::RegisterHandle(HANDLE handle, ULONG_PTR key){

	if (NULL == CreateIoCompletionPort(handle, completionPort, key, NULL)) {
		int32 err = WSAGetLastError();
		HandleError(L"RegisterHandle", err);

		return false;
	}

	return true;
}

bool SocketManager::Bind(SOCKET socket, NetAddress address) {

	if (SOCKET_ERROR == bind(socket, (SOCKADDR*)&address.GetSockAddr(), sizeof(SOCKADDR))) {
		int32 err = WSAGetLastError();
		HandleError(L"Bind Error", err);

		return false;
	}

	return true;
}

bool SocketManager::BindAnyAddress(SOCKET socket, uint16 port) {

	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == bind(socket, (SOCKADDR*)&address, sizeof(SOCKADDR))) {
		int32 err = WSAGetLastError();
		HandleError(L"BindAnyAddress Error", err);

		return false;
	}

	return true;
}

bool SocketManager::Listen(SOCKET socket, int32 backlog) {

	if (SOCKET_ERROR == listen(socket, backlog)) {
		int32 err = WSAGetLastError();
		HandleError(L"Listen Error", err);

		return false;
	}

	return true;
}

bool SocketManager::Send(){
	
	return false;
}

bool SocketManager::Recv()
{
	return false;
}

bool SocketManager::Accept(SOCKET listenSocket, SOCKET AcceptSocket,BYTE* recvBuf, OVERLAPPED* overlapped){
	
	DWORD bytes = 0;
	if (false == AcceptEx(listenSocket, AcceptSocket, recvBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, overlapped)) {
		int32 err = WSAGetLastError();

		if (err != WSA_IO_PENDING) {
			HandleError(L"AcceptEx Error", err);

			return false;
		}
	}

	return true;
}

bool SocketManager::BindWindowFunction(SOCKET dummySocket, GUID guid, LPVOID* fn){

	DWORD bytes = 0;
	if (SOCKET_ERROR == ::WSAIoctl(dummySocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), &bytes, NULL, NULL)) {
		int32 err = WSAGetLastError();
		HandleError(L"WSAStartup Error", err);

		return false;
	}

	return true;
}

void SocketManager::HandleError(wstring msg, int32 errorCode = 0){

	wcout << msg << " : " << errorCode <<endl;
}