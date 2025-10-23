#include "pch.h"
#include "GameSession.h"

#include "PacketHandler.h"
#include "RoomManager.h"
#include "PlayerManager.h"
#include "ActorManager.h"

GameSession::GameSession(Service* owner) : Session(owner){
	
}

GameSession::~GameSession(){
	//cout << "~GameSession() " << endl;
}

void GameSession::OnConnect(){

}

void GameSession::OnSend(int32 sendBytes){
	// cout << "Send : " << sendBytes << endl;
}

void GameSession::OnRecvPacket(const PacketContext& packetContext, int32 recvBytes){

	PacketHandler::HandlePacket(static_pointer_cast<GameSession>(shared_from_this()), packetContext, GetOwner());
}

void GameSession::OnDisconnect(){

	uint64 sessionId = GetSessionId();
	spdlog::info("[GameSession::OnDisconnect] SessionId : {}", sessionId);
	//cout << "[GameSession::OnDisconnect] SessionId : " << sessionId << endl;

	// player data 정리
	GPlayerManager->PushJobGetPosition(sessionId, [sessionId](PlayerTransform position) {
		
		GRoomManager->PushJobLeaveRoom(position._roomId, sessionId);
	});
	GPlayerManager->PushJobRemovePlayer(GetSessionId());
}
