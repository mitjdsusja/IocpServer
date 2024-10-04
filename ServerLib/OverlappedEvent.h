#pragma once
#include "BufferPool.h"

class Session;
class SocketEntity;

enum EventType {

	ACCEPT = 1,
	CONNECT = 2,
	DISCONNECT = 3,
	SEND = 4,
	RECV = 5,
};

class OverlappedEvent : public OVERLAPPED{
public:
	OverlappedEvent(EventType eventType) : OVERLAPPED{ 0 }, _eventType(eventType) { }

public:
	EventType _eventType;
	shared_ptr<SocketEntity> _owner = nullptr;
};

class AcceptEvent : public OverlappedEvent {
public:
	AcceptEvent() : OverlappedEvent(EventType::ACCEPT) { }
	shared_ptr<Session> _session = nullptr;
};

class ConnectEvent : public OverlappedEvent {
public:
	ConnectEvent() : OverlappedEvent(EventType::CONNECT) { }

};

class DisconnectEvent : public OverlappedEvent {
public:
	DisconnectEvent() : OverlappedEvent(EventType::DISCONNECT) { }

};

class SendEvent : public OverlappedEvent {
public:
	SendEvent() : OverlappedEvent(EventType::SEND) { }

	void BufferClear() {
		for (SendBuffer* sendBuffer : _sendBuffers) {
			GSendBufferPool->Push(sendBuffer);
		}
		_sendBuffers.clear();
	}

public:
	vector<SendBuffer*> _sendBuffers;
};

class RecvEvent : public OverlappedEvent {
public:
	RecvEvent() : OverlappedEvent(EventType::RECV) { }

};


