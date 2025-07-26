#pragma once
#include "JobQueue.h"

class MonitorManager : public JobQueueBase{
public:
	bool CreateMonitorProcess();
	bool ConnectPipe();
	bool PushJobSendMsg(const wstring& sendMessage);

private:
	HANDLE hPipe;
};

