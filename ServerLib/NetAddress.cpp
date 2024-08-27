#include "pch.h"
#include "NetAddress.h"

NetAddress::NetAddress(){

}

NetAddress::NetAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr){
	
}

NetAddress::NetAddress(wstring ip, uint16 port){

	SetSockAddr(ip, port);
}

void NetAddress::SetSockAddr(SOCKADDR_IN sockAddr){

	_sockAddr = sockAddr;
}

void NetAddress::SetSockAddr(wstring ip, uint16 port){

	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr = IpToSockAddr(ip.c_str());
	_sockAddr.sin_port = htons(port);
}

wstring NetAddress::GetIpAddress(){

	return wstring();
}

IN_ADDR NetAddress::IpToSockAddr(const WCHAR* ip){

	IN_ADDR address;
	InetPtonW(AF_INET, ip, &address);
	return address;
}

