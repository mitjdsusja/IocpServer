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

	SocketManager::Connect(_peerSocket, (SOCKADDR*)&_peerAddress.GetSockAddr(), &_connectEvent);
}

void Session::RegisterSend(){

	// TODO : Pop SendQueue 
	//
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

	// TODO : 
	if (false == SocketManager::Recv(_peerSocket, _recvBuffer, &_recvEvent)) {

	}
}

void Session::ProcessConnect(OverlappedEvent* event, int32 processBytes){

}

void Session::ProcessSend(OverlappedEvent* event, int32 processBytes){

	if (event->_eventType != EventType::SEND) {
		ErrorHandler::HandleError(L"ProcessSend Error : INVALID EVENT TYPE");
	}
	cout << "[SEND] : Process Send " << endl;
}

void Session::ProcessRecv(OverlappedEvent* event, int32 processBytes){

	if (event->_eventType != EventType::RECV) {
		ErrorHandler::HandleError(L"ProcessRecv Error : INVALID EVENT TYPE");
	}
	BYTE* buffer = _recvBuffer->ReadPos();
	WCHAR msg[100];
	memcpy(msg, buffer, _recvBuffer->DataSize());
	int32 recvLen = processBytes;
	wcout << L"[RECV] RecvLen : " << recvLen << " | " 
		<< "RecvData : " << msg << endl;


	RegisterRecv();
}


