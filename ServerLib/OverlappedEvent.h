#pragma once

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
	SocketEntity* _owner = nullptr;
};

class AcceptEvent : public OverlappedEvent {
public:
	AcceptEvent() : OverlappedEvent(EventType::ACCEPT) { }
	Session* _session = nullptr;
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

};

class RecvEvent : public OverlappedEvent {
public:
	RecvEvent() : OverlappedEvent(EventType::RECV) { }

};


