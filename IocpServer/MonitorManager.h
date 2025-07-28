#pragma once
#include "Actor.h"

class MonitorManager : public Actor{
public:
	bool CreateMonitorProcess();
	bool ConnectPipe();

	void PushJobSendMsg(const wstring& sendMessage);

private:
	void SendMsg(const wstring& sendMessage);

private:
	HANDLE hPipe;
};

