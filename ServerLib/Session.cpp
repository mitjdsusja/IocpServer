#include "pch.h"
#include "Session.h"
#include "PacketHandler.h"

Session::Session(Service* owner) : _owner(owner) {

	_peerSocket = SocketManager::CreateSocket();
	_recvBuffer = new RecvBuffer(RECV_BUFFER_SIZE);
}
Session::~Session(){
	
	if (INVALID_SOCKET != _peerSocket) {
		closesocket(_peerSocket);
	}

	delete _recvBuffer;

	cout << "~Session()" << endl;
}

void Session::Connect(NetAddress peerAddress){

	_peerAddress = peerAddress;
	SocketManager::BindAnyAddress(_peerSocket, 0);

	RegisterConnect(peerAddress);
}

void Session::Send(SendBuffer* sendBuffer){

	// TODO : Send Msg
	{
		lock_guard<mutex> _lock(_mutex);
		sendQueue.push(sendBuffer);

		if (sendRegistered == true) {
			return;
		}
		else {
			sendRegistered = true;
		}
	}

	RegisterSend();
}

void Session::Recv(){

	// TODO : Recv Msg
}

void Session::Disconnect(){

	_owner->removeSession(shared_from_this());

	if (_connectEvent._owner != nullptr) _connectEvent._owner = nullptr;
	if (_sendEvent._owner != nullptr) _sendEvent._owner = nullptr;
	if (_recvEvent._owner != nullptr) _recvEvent._owner = nullptr;
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
		ProcessSend(event, numOfBytes);
		break;
	case EventType::RECV:
		ProcessRecv(event, numOfBytes);
		break;
	default:
		cout << "[DEFAULT] " << endl;
		break;
	}
}

void Session::RegisterConnect(NetAddress& peerAddress){

	if (_connectEvent._owner == nullptr) _connectEvent._owner = shared_from_this();
	SocketManager::Connect(_peerSocket, (SOCKADDR*)&_peerAddress.GetSockAddr(), &_connectEvent);
}

void Session::RegisterSend(){

	if (_sendEvent._owner == nullptr) _sendEvent._owner = shared_from_this();
	
	vector<SendBuffer*> sendBuffers;
	int32 bufferCount;
	{
		lock_guard<mutex> _lock(_mutex);

		bufferCount = sendQueue.size();
		sendBuffers.resize(bufferCount);

		for (int32 i = 0;i < bufferCount;i++) {

			sendBuffers[i] = sendQueue.front();
			sendQueue.pop();
		}
	}

	DWORD bytes = 0;
	if (false == SocketManager::Send(_peerSocket, sendBuffers[0], bufferCount, &_sendEvent)) {
		// TODO : failed send data Process 
		//		  Push SendQueue - RESEND
		for (int32 i = 0;i < bufferCount;i++) {
			Send(sendBuffers[i]);
		}
	}

	sendRegistered = false;
}

void Session::RegisterRecv(){

	if (_recvEvent._owner == nullptr) _recvEvent._owner = shared_from_this();

	if (false == SocketManager::Recv(_peerSocket, _recvBuffer, &_recvEvent)) {
		
	}
}

void Session::ProcessConnect(OverlappedEvent* event, int32 processBytes){

}

void Session::ProcessDisconnect(OverlappedEvent* event, int32 ProcessBytes){

	
}

void Session::ProcessSend(OverlappedEvent* event, int32 processBytes){

	if (event->_eventType != EventType::SEND) {
		ErrorHandler::HandleError(L"ProcessSend Error : INVALID EVENT TYPE");
	}
	cout << "[SEND] : Process Send " << endl;
}

void Session::ProcessRecv(OverlappedEvent* event, int32 recvBytes){

	if (event->_eventType != EventType::RECV) {
		ErrorHandler::HandleError(L"ProcessRecv Error : INVALID EVENT TYPE");
		return;
	}

	if (recvBytes == 0) {
		Disconnect();
		return;
	}

	_recvBuffer->Write(recvBytes);

	OnRecv(_recvBuffer, recvBytes);

	RegisterRecv();
}

void Session::CleanResource(){
	Disconnect();
}

int32 ServerSession::OnRecv(RecvBuffer* recvBuffer, int32 recvBytes){

	if (recvBytes < sizeof(PacketHeader)) {
		ASSERT_CRASH(false);
	}

	int32 processLen = 0;
	while (true) {
		BYTE* buffer = recvBuffer->ReadPos();
		PacketHeader* header = (PacketHeader*)buffer;

		if (recvBuffer->DataSize() < header->packetSize) {
			break;
		}

		PacketHandler::HandlePacket(header);
		recvBuffer->Read(header->packetSize);

		processLen += header->packetSize;
	}
	return processLen;
}

void ServerSession::OnSend(int32 sendBytes){

}

int32 ClientSession::OnRecv(RecvBuffer* recvBuffer, int32 recvBytes){

	int32 processLen = 0;

	return processLen;
}

void ClientSession::OnSend(int32 sendBytes){

}
