#include "pch.h"
#include "GameSession.h"
#include "PacketHandler.h"

GameSession::GameSession(Service* owner) : Session(owner){

}

GameSession::~GameSession(){
	cout << "~GameSession() " << endl;
}

void GameSession::OnSend(int32 sendBytes){
	//cout << "Send" << endl;
}

void GameSession::OnRecvPacket(BYTE* recvBuffer, int32 recvBytes){

	PacketHeader* header = (PacketHeader*)recvBuffer;
	PacketHandler::HandlePacket(shared_from_this(), header, GetOwner());
}
