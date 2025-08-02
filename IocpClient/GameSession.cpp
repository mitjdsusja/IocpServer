#include "pch.h"
#include "Global.h"

#include "GameSession.h"
#include "PacketHandler.h"
#include "Player.h"

GameSession::GameSession(Service* owner) : Session(owner) {

}

GameSession::~GameSession() {
	cout << "~GameSession() " << endl;
}

void GameSession::OnConnect() {

	GPlayerManager->CreatePlayerAndAdd(shared_from_this());
}

void GameSession::OnSend(int32 sendBytes) {
	//cout << "Send" << endl;
}

void GameSession::OnRecvPacket(BYTE* recvBuffer, int32 recvBytes) {

	PacketHeader* header = (PacketHeader*)recvBuffer;
	PacketHandler::HandlePacket(static_pointer_cast<GameSession>(shared_from_this()), header, GetOwner());
}
