#pragma once
#include "JobQueue.h"

class MonitorManager : public JobQueueBase{
public:
	bool CreateMonitorProcess();
	bool ConnectPipe();

	void PushJobSendMsg(const wstring& sendMessage);

private:
	void SendMsg(const wstring& sendMessage);

private:
	HANDLE hPipe;
};

