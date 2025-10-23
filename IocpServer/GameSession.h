#pragma once
#include "Session.h"

class GameSession : public Session{
public :
	GameSession(Service* owner);
	virtual ~GameSession();

	virtual void OnConnect() override;
	virtual void OnSend(int32 sendBytes) override;
	virtual void OnRecvPacket(const PacketContext& packetContext, int32 recvBytes) override;
	virtual void OnDisconnect() override;

	void SetDbId(int64 userNum) { _dbId = userNum; }
	int64 GetDbId() { return _dbId; }

private:
	int64 _dbId = 0;
	
};

