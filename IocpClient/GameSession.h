#pragma once
#include "Session.h"

class GameSession : public Session {
public:
	GameSession(Service* owner);
	virtual ~GameSession();

	virtual void OnConnect() override;
	virtual void OnSend(int32 sendBytes) override;
	virtual void OnRecvPacket(const PacketContext& packetContext, int32 recvBytes) override;

};

