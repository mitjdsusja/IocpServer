#pragma once

class NetAddress{
public:
	NetAddress();
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(wstring ip, uint16 port);

	void SetSockAddr(SOCKADDR_IN sockAddr);
	void SetSockAddr(wstring ip, uint16 port);

	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	wstring GetIpAddress();
	uint16 GetPort() { return ::ntohs(_sockAddr.sin_port); }

private:
	IN_ADDR IpToSockAddr(const WCHAR* ip);

private:
	SOCKADDR_IN _sockAddr = {};
};

