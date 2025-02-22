#include "pch.h"
#include "GameSession.h"
#include "PacketHandler.h"

#include "RoomManager.h"

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

	shared_ptr<Player> player = GPlayerManager->GetPlayer(GetSessionId());
	PlayerInfo playerInfo = player->GetPlayerInfo();
	

	GRoomManager->RemovePlayerFromRoom(playerInfo._roomId, GetSessionId());
	GPlayerManager->RemovePlayer(GetSessionId());

}
