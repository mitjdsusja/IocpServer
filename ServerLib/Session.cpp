#include "pch.h"
#include "Session.h"

Session::Session() {

	_peerSocket = SocketManager::CreateSocket();
	_recvBuffer = new RecvBuffer(RECV_BUFFER_SIZE);
}

Session::~Session(){

	if (INVALID_SOCKET != _peerSocket) {
		closesocket(_peerSocket);
	}

	delete _recvBuffer;
}

void Session::Connect(NetAddress peerAddress){

	_address = peerAddress;
	SocketManager::BindAnyAddress(_peerSocket, 0);

	DWORD bytes = 0;
	SocketManager::Connect(_peerSocket, (SOCKADDR*)&_address.GetSockAddr(), &_connectEvent);
}

void Session::Send(){

}

void Session::Recv(){

}

void Session::RegisterConnect(){

}

void Session::RegisterSend(){

}

void Session::RegisterRecv(){

}
