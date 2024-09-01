#pragma once
class SocketEntity{
public:
	virtual void Process(OverlappedEvent* event, int32 numOfBytes) abstract;

private:

};

