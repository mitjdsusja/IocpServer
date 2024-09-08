#include "pch.h"
#include "Listener.h"

Listener::Listener(Service* owner) : _owner(owner){

	_listenSocket = SocketManager::CreateSocket();
}

Listener::~Listener(){

	if (INVALID_SOCKET != _listenSocket) {
		closesocket(_listenSocket);
	}
}

void Listener::SetEnv(NetAddress myAddress){

	_myAddress = myAddress;

	ASSERT_CRASH(false == SocketManager::Bind(_listenSocket, _myAddress));
	ASSERT_CRASH(false == SocketManager::Listen(_listenSocket));
}

void Listener::Start(int32 acceptCount){

	for (int32 i = 0;i < acceptCount;i++) {

		AcceptEvent* acceptEvent = new AcceptEvent();
		acceptEvent->_owner = this;
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent) {

	Session* session = acceptEvent->_session;
	acceptEvent->_session = nullptr;

	SOCKET socket = session->GetSocket();
	if (socket == INVALID_SOCKET) {

		delete session;
		RegisterAccept(acceptEvent);
		return;
	}

	if (SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&_listenSocket, sizeof(_listenSocket))) {

		int32 err = WSAGetLastError();
		ErrorHandler::HandleError(L"setsockopt Error : ", err);

		delete session;
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddr;
	int32 addrLen = sizeof(sockAddr);
	if (SOCKET_ERROR == getpeername(socket, (SOCKADDR*)&sockAddr, &addrLen)) {
		int32 err = WSAGetLastError();
		ErrorHandler::HandleError(L"setsockopt Error : ", err);

		delete session;
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetPeerAddress(NetAddress(sockAddr));
	_owner->RegisterHandle((HANDLE)session->GetSocket());

	OnAccept(session);

	session->RegisterRecv();
	// Service add session

	RegisterAccept(acceptEvent);
}


void Listener::Process(OverlappedEvent* event, int32 numOfBytes){

	if (event->_eventType != EventType::ACCEPT) {

		ErrorHandler::HandleError(L"Listener Process EventType Error");
	}

	AcceptEvent* acceptEvent = (AcceptEvent*)event;
	ProcessAccept(acceptEvent);
	
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent){

		Session* peerSession = new Session();
		RecvBuffer* recvBuffer = peerSession->GetRecvBuffer();
		acceptEvent->_session = peerSession;

		SocketManager::Accept(_listenSocket, peerSession->GetSocket(), recvBuffer->WritePos(), acceptEvent);
}

void Listener::OnAccept(Session* session){

	NetAddress peerAddress = session->GetPeerAddressRef();
	wcout << L"[ACCEPT] " << "Address : " << peerAddress.GetIpAddress() << " Port : " << peerAddress.GetPort() << endl;
}

