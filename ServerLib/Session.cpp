#include "pch.h"
#include "Session.h"

Session::Session(NetAddress address) : _address(address){

	_peerSocket = SocketManager::CreateSocket();
	_recvBuffer = new RecvBuffer(RECV_BUFFER_SIZE);
}

Session::~Session(){

	closesocket(_peerSocket);
	delete _recvBuffer;
}

void Session::Accept(){

}

void Session::Connect(){

}

void Session::Send(){

}

void Session::Recv(){

}

void Session::RegisterAccept(){

}

void Session::RegisterConnect(){

}

void Session::RegisterSend(){

}

void Session::RegisterRecv(){

}
