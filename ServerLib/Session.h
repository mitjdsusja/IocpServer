#pragma once

class Session{
public:
	void Accept();
	void Connect();
	void Send();
	void Recv();

public:
	void RegisterAccept();
	void RegisterConnect();
	void RegisterSend();
	void RegisterRecv();

private:
	SOCKET* peerSocket;

private:
};

