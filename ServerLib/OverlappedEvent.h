#pragma once

class Session;

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
	Session* session;
};

class AcceptEvent : public OverlappedEvent {
public:
	AcceptEvent() : OverlappedEvent(EventType::ACCEPT) { }

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


