#pragma once
#include "Session.h"
#include "PlayerManager.h"

class GameSession : public Session{
public :
	GameSession(Service* owner);
	virtual ~GameSession();

	virtual void OnSend(int32 sendBytes) override;
	virtual void OnRecvPacket(BYTE* recvBuffer, int32 recvBytes) override;

	void SetUserNum(int32 userNum) { _userNum = userNum; }
	int32 GetUserNum() { return _userNum; }

private:
	// DB에 저장된 데이터 번호 (DB에 저장된 유저정보를 찾기위해 UserID대신 
	int32 _userNum = 0;
};

