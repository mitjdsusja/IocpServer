#include "pch.h"
#include "Session.h"
#include "PacketHeader.h"
#include "PacketFrame.h"
#include "PacketContext.h"
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

	_sendEvent._eventStartTimePoint = chrono::steady_clock::now();

	DWORD bytes = 0;
	if (false == SocketManager::Send(_peerSocket, sendBuffers, &_sendEvent)) {
		
		// Send 실패 시 큐에 다시 넣기
		{
			lock_guard<mutex> _lock(_sendQueueMutex);

			// TODO : 
			// 실패 시 Queue에 Push함.
			// Push전에 다른 스레드가 Queue에 넣었을 수도 있으므로 순서 주의
			// Dequeue로 수정하여 front에 붙이도록 수정 필요
			// 혹은 실패 시 연결 종료
			for (int32 i = 0; i < bufferCount; i++) {
				_sendQueue.push(sendBuffers[i]);
			}

			_sendRegistered = false;
		}
		
		_sendEvent._sendBuffers.clear();

		RegisterSend();
	}
}

void Session::RegisterRecv(){

	if (_recvEvent._owner == nullptr) _recvEvent._owner = shared_from_this();

	if (false == SocketManager::Recv(_peerSocket, _recvBuffer, &_recvEvent)) {
		
	}
}

void Session::ProcessConnect(OverlappedEvent* event, int32 processBytes){

	spdlog::info("[Session::ProcessConnect] Connect Server : {}", GetSessionId());
	//cout << "[Connect] Connect Server : " << GetSessionId() << endl;

	OnConnect();
	RegisterRecv();
}

void Session::ProcessDisconnect(OverlappedEvent* event, int32 ProcessBytes){

	
}

void Session::ProcessSend(OverlappedEvent* event, int32 processBytes){

	if (event->_eventType != EventType::SEND) {

		spdlog::info("[Session::ProcessSend] ProcessSend Error : INVALID EVENT TYPE");
		//ErrorHandler::HandleError(L"ProcessSend Error : INVALID EVENT TYPE");
	}

	//spdlog::info("[Session::ProcessSend] SessionId {}, Send Len : {}", GetSessionId(), processBytes);

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

		spdlog::info("[Session::ProcessRecv] ProcessRecv Error : INVALID EVENT TYPE");
		//ErrorHandler::HandleError(L"ProcessRecv Error : INVALID EVENT TYPE");
		return;
	}

	if (recvBytes == 0) {

		spdlog::info("[Session::ProcessRecv] Recv 0 Bytes");
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

	int32 processLen = 0;

	while (true) {

		BYTE* cur = recvBuffer + processLen;
		int32 remainSize = dataSize - processLen;

		// --------데이터 파싱---------
		PacketHeader::View headerView = {};
		if (PacketHeader::TryParse(cur, remainSize, headerView) == false)  {
			break;
		}

		if (headerView.bodySize < 0) {
			return -1;	// Disconnect
		}

		PacketFrame::View frameView = {};
		if(PacketFrame::TryParse(cur + sizeof(PacketHeader), headerView.bodySize, frameView) == false) {
			break;
		}

		if(frameView.totalFrameCount <= 0 || frameView.frameIndex < 0 || frameView.frameIndex >= frameView.totalFrameCount) {
			return -1;	// Disconnect
		}

		// ---------Frame 조립---------
		
		// 기존에 존재하지 않는 FrameId인 경우 저장소 새로 생성
		if (_recvFrames.find(frameView.frameId) == _recvFrames.end()) {

			_recvFrames[frameView.frameId] = vector<vector<BYTE>>(frameView.totalFrameCount);
			_recvFrameCounts[frameView.frameId] = 0;
		}

		vector<BYTE> data(frameView.payload, frameView.payload + frameView.payloadSize);
		_recvFrames[frameView.frameId][frameView.frameIndex] = std::move(data);
		_recvFrameCounts[frameView.frameId]++;

		// 모든 frame이 도착한 경우 패킷 조립
		if (_recvFrameCounts[frameView.frameId] == frameView.totalFrameCount) {
			
			PacketContext packetContext = {};

			packetContext.header.packetId = headerView.id;
			packetContext.header.packetSize = 0;	// 임시 값
			
			for (int32 curFrameIndex = 0; curFrameIndex < frameView.totalFrameCount; ++curFrameIndex) {

				auto& part = _recvFrames[frameView.frameId][curFrameIndex];
				packetContext.dataVector.insert(packetContext.dataVector.end(), part.begin(), part.end());
			}
			packetContext.header.packetSize = sizeof(PacketHeader) + static_cast<int32>(packetContext.dataVector.size());

			OnRecvPacket(packetContext, packetContext.header.packetSize);

			_recvFrames.erase(frameView.frameId);
			_recvFrameCounts.erase(frameView.frameId);
		}

		processLen += headerView.size;
		if (processLen >= dataSize) {
			break;
		}
	}
	return processLen;
}
