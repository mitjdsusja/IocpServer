#include "pch.h"
#include "GameSession.h"
#include "PacketHandler.h"
#include "RoomManager.h"
#include "PlayerManager.h"

GameSession::GameSession(Service* owner) : Session(owner){
	
}

GameSession::~GameSession(){
	//cout << "~GameSession() " << endl;
}

void GameSession::OnConnect(){

}

void GameSession::OnSend(int32 sendBytes){
	//cout << "Send" << endl;
}

void GameSession::OnRecvPacket(BYTE* recvBuffer, int32 recvBytes){

	PacketHeader* header = (PacketHeader*)recvBuffer;
	PacketHandler::HandlePacket(static_pointer_cast<GameSession>(shared_from_this()), header, GetOwner());
}

void GameSession::OnDisconnect(){

	uint64 sessionId = GetSessionId();
	cout << "[DISCONNECT] SessionId : " << GetSessionId() << endl;

	// player data 정리
	// - 방에서 나가기
	// - PlayerManager에서 제거
}
