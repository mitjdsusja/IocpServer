#include "pch.h"
#include "GameSession.h"
#include "PacketHandler.h"

#include "RoomManager.h"

GameSession::GameSession(Service* owner) : Session(owner){
	
}

GameSession::~GameSession(){
	//cout << "~GameSession() " << endl;
}

void GameSession::OnConnect(){

	GPlayerManager->CreateAndAddPlayer(static_pointer_cast<GameSession>(shared_from_this()), GetSessionId());
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

	shared_ptr<Player> player = GPlayerManager->GetPlayer(GetSessionId());
	if (player == nullptr) {

		//cout << "INVALID PLAYERID" << endl;
	}
	else {

		PlayerInfo playerInfo = player->GetPlayerInfo();
		GRoomManager->LeavePlayerFromRoom(playerInfo._roomId, GetSessionId());
	}

	GPlayerManager->RemovePlayer(GetSessionId());

}
