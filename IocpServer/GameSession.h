#pragma once
#include "Session.h"

class GameSession : public Session{
public :
	GameSession(Service* owner);
	virtual ~GameSession();

	virtual void OnSend(int32 sendBytes) override;
	virtual void OnRecvPacket(BYTE* recvBuffer, int32 recvBytes) override;

	void SetUserNum(int32 userNum) { _userNum = userNum; }
	int32 GetUserNum() { return _userNum; }

private:
	int32 _userNum = 0;
};

