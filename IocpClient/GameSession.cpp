#include "pch.h"
#include "Global.h"

#include "DummyClientService.h"
#include "GameSession.h"
#include "PacketHandler.h"
#include "Player.h"

GameSession::GameSession(Service* owner) : Session(owner) {

}

GameSession::~GameSession() {
	cout << "~GameSession() " << endl;
}

void GameSession::OnConnect() {

	DummyClientService* dummyClientService = (DummyClientService*)GetOwner();
	dummyClientService->AddConnectedSessionCount();
}

void GameSession::OnSend(int32 sendBytes) {
	//cout << "Send" << endl;
}

void GameSession::OnRecvPacket(const PacketHeader::View& packetHeaderView, int32 recvBytes) {

	PacketHandler::HandlePacket(static_pointer_cast<GameSession>(shared_from_this()), packetHeaderView, GetOwner());
}
