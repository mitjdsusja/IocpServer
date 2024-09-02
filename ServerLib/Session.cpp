#include "pch.h"
#include "Session.h"

Session::Session() {

	_peerSocket = SocketManager::CreateSocket();
	_recvBuffer = new RecvBuffer(RECV_BUFFER_SIZE);

	// PTR
	_sendEvent._owner = this;
	_recvEvent._owner = this;
	_connectEvent._owner = this;
}

Session::~Session(){

	if (INVALID_SOCKET != _peerSocket) {
		closesocket(_peerSocket);
	}

	delete _recvBuffer;
}

void Session::Connect(NetAddress peerAddress){

	_peerAddress = peerAddress;
	SocketManager::BindAnyAddress(_peerSocket, 0);

	DWORD bytes = 0;
	SocketManager::Connect(_peerSocket, (SOCKADDR*)&_peerAddress.GetSockAddr(), &_connectEvent);
}

void Session::Send(){

	// TODO : Send Msg
}

void Session::Recv(){

	// TODO : Recv Msg
}

void Session::Process(OverlappedEvent* event, int32 numOfBytes){

	// TODO  : Mapping Function
	switch (event->_eventType) {
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

void Session::RegisterConnect(){

	// TODO : 
}

void Session::RegisterSend(){

	// TODO : 
}

void Session::RegisterRecv(){

	// TODO : 
}


