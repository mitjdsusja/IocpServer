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
		_sendQueue.push(sendBuffer);

		if (_sendRegistered == true) {
			return;
		}
		else {
			_sendRegistered = true;
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

		bufferCount = _sendQueue.size();
		sendBuffers.resize(bufferCount);

		for (int32 i = 0;i < bufferCount;i++) {

			sendBuffers[i] = _sendQueue.front();
			_sendQueue.pop();
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

	_sendRegistered = false;
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
	_recvBuffer->Read(OnRecv(_recvBuffer->ReadPos(), recvBytes));

	RegisterRecv();
}

void Session::CleanResource(){
	Disconnect();
}

ServerSession::ServerSession(Service* owner)
	: Session(owner){

}

ServerSession::~ServerSession(){

}

int32 ServerSession::OnRecv(BYTE* recvBuffer, int32 recvBytes){
	
	if (recvBytes < sizeof(PacketHeader)) {
		ASSERT_CRASH(false);
	}

	BYTE* buffer = recvBuffer;
	int32 processLen = 0;
	while (true) {
		buffer = recvBuffer + processLen;
		PacketHeader* header = (PacketHeader*)buffer;

		// TODO : Validate
		if (recvBytes < header->packetSize) {
			break;
		}
		PacketHandler::HandlePacket(header);

		processLen += header->packetSize;
		if (processLen >= recvBytes) {
			break;
		}
	}
	return processLen;
}

void ServerSession::OnSend(int32 sendBytes){

}

ClientSession::ClientSession(Service* owner)
	: Session(owner){

}

ClientSession::~ClientSession(){

}

int32 ClientSession::OnRecv(BYTE* recvBuffer, int32 recvBytes){

	if (recvBytes < sizeof(PacketHeader)) {
		ASSERT_CRASH(false);
	}

	BYTE* buffer = recvBuffer;
	int32 processLen = 0;
	while (true) {
		buffer = recvBuffer + processLen;
		PacketHeader* header = (PacketHeader*)buffer;

		// TODO : Validate
		if (recvBytes < header->packetSize) {
			break;
		}
		PacketHandler::HandlePacket(header);

		processLen += header->packetSize;
		if (processLen >= recvBytes) {
			break;
		}
		cout << processLen << endl;
	}
	return processLen;
}

void ClientSession::OnSend(int32 sendBytes){

}
