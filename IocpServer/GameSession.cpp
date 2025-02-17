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
	PacketHandler::HandlePacket(static_pointer_cast<GameSession>(shared_from_this()), header, GetOwner());
}

void GameSession::OnDisconnect(){

	cout << "DISCONNECT" << endl;

	GPlayerManager->RemovePlayer(GetSessionId());

}
