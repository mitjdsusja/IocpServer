#pragma once

class MonitorManager{
public:
	bool CreateMonitorProcess();
	bool ConnectPipe();
	bool SendMsg(const wstring& sendMessage);

private:
	HANDLE hPipe;
};

