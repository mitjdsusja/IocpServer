#include "pch.h"
#include "Listener.h"

class ClientSession;

Listener::Listener(Service* owner) : _owner(owner){

	_listenSocket = SocketManager::CreateSocket();
}

Listener::~Listener(){
	_owner = nullptr;

	for (AcceptEvent* acceptEvent : _acceptEvents) {
		delete acceptEvent;
	}

	if (INVALID_SOCKET != _listenSocket) {
		closesocket(_listenSocket);
	}
}

void Listener::Init(NetAddress myAddress){

	_myAddress = myAddress;

	ASSERT_CRASH(false == SocketManager::Bind(_listenSocket, _myAddress));
	ASSERT_CRASH(false == SocketManager::Listen(_listenSocket));
}

void Listener::Start(int32 acceptCount){

	for (int32 i = 0;i < acceptCount;i++) {

		AcceptEvent* acceptEvent = new AcceptEvent();
		acceptEvent->_owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent) {

	shared_ptr<Session> session = acceptEvent->_session;
	acceptEvent->_session = nullptr;

	SOCKET socket = session->GetSocket();
	if (socket == INVALID_SOCKET) {

		RegisterAccept(acceptEvent);
		return;
	}

	if (SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&_listenSocket, sizeof(_listenSocket))) {

		int32 err = WSAGetLastError();
		ErrorHandler::HandleError(L"setsockopt Error : ", err);

		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddr;
	int32 addrLen = sizeof(sockAddr);
	if (SOCKET_ERROR == getpeername(socket, (SOCKADDR*)&sockAddr, &addrLen)) {
		int32 err = WSAGetLastError();
		ErrorHandler::HandleError(L"setsockopt Error : ", err);

		RegisterAccept(acceptEvent);
		return;
	}

	session->SetPeerAddress(NetAddress(sockAddr));
	_owner->RegisterHandle((HANDLE)session->GetSocket());

	OnAccept(session);

	_owner->AddSession(session);
	session->RegisterRecv();

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
		
	shared_ptr<Session> session = _owner->CreateSession();
	session->SetOwner(_owner);
	RecvBuffer* recvBuffer = session->GetRecvBuffer();
	acceptEvent->_session = session;

	SocketManager::Accept(_listenSocket, session->GetSocket(), recvBuffer->WritePos(), acceptEvent);
}

void Listener::OnAccept(shared_ptr<Session> session) {

	NetAddress peerAddress = session->GetPeerAddressRef();
	wcout << L"[ACCEPT] " << "Address : " << peerAddress.GetIpAddress() << " Port : " << peerAddress.GetPort() << endl;
}

void Listener::CleanResource(){
	
	// TOOD : _owner에서 this ptr 제거
	_owner = nullptr;
	
	for (AcceptEvent* event : _acceptEvents) {

		event->_owner = nullptr;
		delete event;
	}
}

