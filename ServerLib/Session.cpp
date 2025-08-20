#include "pch.h"
#include "Session.h"
#include "PacketHeader.h"
#include "Service.h"


Session::Session(Service* owner) : _owner(owner) {

	_peerSocket = SocketManager::CreateSocket();
	_recvBuffer = new RecvBuffer(RECV_BUFFER_SIZE);

}
Session::~Session(){
	
	if (INVALID_SOCKET != _peerSocket) {
		closesocket(_peerSocket);
	}

	delete _recvBuffer;

	//cout << "~Session()" << endl;
}

void Session::Connect(NetAddress peerAddress){

	_peerAddress = peerAddress;
	SocketManager::BindAnyAddress(_peerSocket, 0);

	RegisterConnect(peerAddress);
}

void Session::Send(shared_ptr<Buffer> sendBuffer){

	if (_isConnected == false) return;

	{
		lock_guard<mutex> _lock(_sendQueueMutex);
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

	OnDisconnect();

	if (_connectEvent._owner != nullptr) _connectEvent._owner = nullptr;
	if (_sendEvent._owner != nullptr) _sendEvent._owner = nullptr;
	if (_recvEvent._owner != nullptr) _recvEvent._owner = nullptr;

	_owner->removeSession(static_cast<shared_ptr<Session>>(shared_from_this()));
}

void Session::Process(OverlappedEvent* event, int32 numOfBytes){

	// TODO  : Mapping Function
	switch (event->_eventType) {
	case EventType::CONNECT:
		ProcessConnect(event, numOfBytes);
		break;
	case EventType::DISCONNECT:
		spdlog::info("[DISCONNECT]");
		//cout << "[DISCONNECT] " << endl;
		break;
	case EventType::SEND:
		ProcessSend(event, numOfBytes);
		break;
	case EventType::RECV:
		ProcessRecv(event, numOfBytes);
		break;
	default:
		spdlog::info("[DEFAULT]");
		//cout << "[DEFAULT] " << endl;
		break;
	}
}

void Session::RegisterConnect(NetAddress& peerAddress){

	if (_connectEvent._owner == nullptr) _connectEvent._owner = shared_from_this();
	SocketManager::Connect(_peerSocket, (SOCKADDR*)&_peerAddress.GetSockAddr(), &_connectEvent);
}

void Session::RegisterSend(){
	
	if (_sendEvent._owner == nullptr) _sendEvent._owner = shared_from_this();
	
	vector<shared_ptr<Buffer>> sendBuffers;
	int32 bufferCount;
	{
		lock_guard<mutex> _lock(_sendQueueMutex);

		bufferCount = (int32)_sendQueue.size();
		sendBuffers.resize(bufferCount);

		for (int32 i = 0;i < bufferCount;i++) {

			sendBuffers[i] = _sendQueue.front();
			_sendEvent._sendBuffers.push_back(sendBuffers[i]);
			_sendQueue.pop();
		}
	}

	// Packet Header Check
	for (auto& buffer : sendBuffers) {

		PacketHeader* header = (PacketHeader*)buffer->GetBuffer();
		spdlog::info("Send to {} Header ID {} size {} ",GetSessionId(), ntohl(header->packetId), ntohl(header->packetSize));
	}

	_sendEvent._eventStartTimePoint = chrono::steady_clock::now();

	DWORD bytes = 0;
	if (false == SocketManager::Send(_peerSocket, sendBuffers, &_sendEvent)) {
		// TODO : failed send data Process 
		//		  Push SendQueue - RESEND

		_sendEvent._sendBuffers.clear();
		for (int32 i = 0;i < bufferCount;i++) {
			Send(sendBuffers[i]);
		}
	}
}

void Session::RegisterRecv(){

	if (_recvEvent._owner == nullptr) _recvEvent._owner = shared_from_this();

	if (false == SocketManager::Recv(_peerSocket, _recvBuffer, &_recvEvent)) {
		
	}
}

void Session::ProcessConnect(OverlappedEvent* event, int32 processBytes){

	spdlog::info("[Connect] Connect Server : {}", GetSessionId());
	//cout << "[Connect] Connect Server : " << GetSessionId() << endl;

	OnConnect();
	RegisterRecv();
}

void Session::ProcessDisconnect(OverlappedEvent* event, int32 ProcessBytes){

	
}

void Session::ProcessSend(OverlappedEvent* event, int32 processBytes){

	if (event->_eventType != EventType::SEND) {

		spdlog::info("ProcessSend Error : INVALID EVENT TYPE");
		//ErrorHandler::HandleError(L"ProcessSend Error : INVALID EVENT TYPE");
	}

	//spdlog::info("[Session::ProcessSend] Send Len : {}", processBytes);

	SendEvent* sendEvent = (SendEvent*)event;

	uint64 eventTime = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - sendEvent->_eventStartTimePoint).count();

	sendEvent->BufferClear();

	uint64 clearTime = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - sendEvent->_eventStartTimePoint).count();
	//spdlog::info("Event CompleteTime : {}", eventTime);
	//spdlog::info("Buffer 반환 : {}", clearTime);

	OnSend(processBytes);

	{
		lock_guard<mutex> _lock(_sendQueueMutex);
		if (_sendQueue.empty()) {
			_sendRegistered = false;
			return;
		}
	}
	
	RegisterSend();
	//cout << "[SEND] : Process Send : " << processBytes << endl;
}

void Session::ProcessRecv(OverlappedEvent* event, int32 recvBytes){
	
	if (event->_eventType != EventType::RECV) {

		spdlog::info("ProcessRecv Error : INVALID EVENT TYPE");
		//ErrorHandler::HandleError(L"ProcessRecv Error : INVALID EVENT TYPE");
		return;
	}

	if (recvBytes == 0) {
		Disconnect();
		return;
	}

	_recvBuffer->Write(recvBytes);
	_recvBuffer->Read(OnRecv(_recvBuffer->ReadPos(), _recvBuffer->DataSize()));

	RegisterRecv();
}

void Session::CleanResource(){
	Disconnect();
}

int32 Session::OnRecv(BYTE* recvBuffer, int32 dataSize){

	if (dataSize < sizeof(PacketHeader)) {
		ASSERT_CRASH(false);
	}

	int32 processLen = 0;

	while (true) {

		BYTE* buffer = recvBuffer + processLen;
		// header만큼 recv안됨.
		if (dataSize - processLen < sizeof(PacketHeader)) {
			break;
		}

		PacketHeader* header = (PacketHeader*)buffer;
		int32 headerPacketId = ntohl(header->packetId);
		int32 headerPacketSize = ntohl(header->packetSize);

		// 전체 packet이 안옴.
		if (dataSize - processLen < headerPacketSize) {
			break;
		}

		// Parse Frame
		PacketFrame* frame = (PacketFrame*)((BYTE*)header + sizeof(PacketHeader));

		int32 framePacketId = ntohl(frame->packetId);
		int32 totalFrameCount = ntohl(frame->totalFrameCount);
		int32 frameIndex = ntohl(frame->frameIndex);

		if (frameIndex < 0 || frameIndex >= totalFrameCount) {

			spdlog::info("INVALID FRAME  Frame: {} / {}", frameIndex, totalFrameCount);
			//wcout << "INVALID FRAME  Frame: " << frameIndex << "/" << totalFrameCount << endl;
			break;
		}

		// packetSize 최소 길이 검증
		if (headerPacketSize < sizeof(PacketHeader) + sizeof(PacketFrame)) {

			spdlog::error("Invalid packet size : {}", headerPacketSize);
			break;
		}

		BYTE* payload = (BYTE*)(frame + 1);
		int32 payloadSize = headerPacketSize - sizeof(PacketHeader) - sizeof(PacketFrame);

		// payloadSize 음수, 너무 큼 방지
		if (payloadSize < 0 || payloadSize >(dataSize - processLen - sizeof(PacketHeader) - sizeof(PacketFrame))) {

			spdlog::error("Packet : {} {} Invalid payload size : {}", headerPacketId, headerPacketSize, payloadSize);
			break;
		}

		vector<BYTE> data(payload, payload + payloadSize);

		if (_recvFrames.find(framePacketId) == _recvFrames.end()) {

			_recvFrames[framePacketId] = vector<vector<BYTE>>(totalFrameCount);
			_recvFrameCounts[framePacketId] = 0;
		}

		_recvFrames[framePacketId][frameIndex] = std::move(data);
		_recvFrameCounts[framePacketId]++;

		if (_recvFrameCounts[framePacketId] == totalFrameCount) {
			
			vector<BYTE> finalPacket(sizeof(PacketHeader));

			PacketHeader* finalHeader = (PacketHeader*)finalPacket.data();
			
			finalHeader->packetId = headerPacketId;
			finalHeader->packetSize = headerPacketSize;
			
			for (int32 i = 0; i < totalFrameCount; ++i) {

				auto& part = _recvFrames[framePacketId][i];
				finalPacket.insert(finalPacket.end(), part.begin(), part.end());
			}
			OnRecvPacket(finalPacket.data(), finalPacket.size());

			_recvFrames.erase(framePacketId);
			_recvFrameCounts.erase(framePacketId);
		}

		processLen += headerPacketSize;
		if (processLen >= dataSize) {
			break;
		}
	}
	return processLen;
}
