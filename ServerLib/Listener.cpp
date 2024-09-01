#include "pch.h"
#include "Listener.h"

Listener::Listener(){

	_listenSocket = SocketManager::CreateSocket();

}

Listener::~Listener(){

	if (INVALID_SOCKET != _listenSocket) {
		closesocket(_listenSocket);
	}
}

bool Listener::SetEnv(NetAddress myAddress){

	_myAddress = myAddress;

	if (false == SocketManager::Bind(_listenSocket, _myAddress)) return false;
	if (false == SocketManager::Listen(_listenSocket)) return false;

	return true;
}

void Listener::Start(int32 acceptCount){

	for (int32 i = 0;i < acceptCount;i++) {

		AcceptEvent* acceptEvent = new AcceptEvent();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent){

		Session* peerSession = new Session();
		RecvBuffer* recvBuffer = peerSession->GetRecvBuffer();
		acceptEvent->session = peerSession;

		SocketManager::Accept(_listenSocket, peerSession->GetSocket(), recvBuffer->WritePos(), acceptEvent);
}
