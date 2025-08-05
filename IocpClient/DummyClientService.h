#pragma once
#include "Service.h"

class DummyClientService : public ClientService{
public:
	DummyClientService(NetAddress address, int32 maxSessionCount, function<shared_ptr<Session>(void)> sessionCreateFunc);

	void LoginAllSession();
	void RequestRoomInfoAndEnterRoom();
	void AddConnectedSessionCount();

	uint32 GetConnectedSessionCount();

private:
	atomic<uint32> _connectedSessionCount = 0;
};

