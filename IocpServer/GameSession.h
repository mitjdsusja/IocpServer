#pragma once
#include "Session.h"
#include "PlayerManager.h"

class GameSession : public Session{
public :
	GameSession(Service* owner);
	virtual ~GameSession();

	virtual void OnSend(int32 sendBytes) override;
	virtual void OnRecvPacket(BYTE* recvBuffer, int32 recvBytes) override;

	void SetDbId(int64 userNum) { _dbId = userNum; }
	int64 GetDbId() { return _dbId; }

private:
	int64 _dbId = 0;
	
};

