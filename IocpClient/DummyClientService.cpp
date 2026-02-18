#include "pch.h"
#include "DummyClientService.h"
#include "GameSession.h"
#include "PacketHandler.h"

#include "messageTest.pb.h"

DummyClientService::DummyClientService(NetAddress address, int32 maxSessionCount, function<shared_ptr<Session>(void)> sessionCreateFunc)
: ClientService(address, maxSessionCount, sessionCreateFunc){

}

void DummyClientService::LoginAllSession(){

	msgTest::CS_Login_Request sendPacketLoginrequest;

	wcout << _sessions.size() << endl;
	for (auto& p : _sessions) {

		shared_ptr<Session>& session = p.second;

		sendPacketLoginrequest.set_id("bot" + to_string(session->GetSessionId() + 1));
		sendPacketLoginrequest.set_password("bot" + to_string(session->GetSessionId() + 1));
		
		spdlog::info("Request Login : bot{}", to_string(session->GetSessionId() + 1));

		const shared_ptr<Buffer> sendBuffer = PacketHandler::MakeSendBuffer(sendPacketLoginrequest, PacketId::PKT_CS_LOGIN_REQUEST);

		session->Send(sendBuffer);
	}
}

void DummyClientService::AddConnectedSessionCount(){

	_connectedSessionCount.fetch_add(1);
}

uint32 DummyClientService::GetConnectedSessionCount(){

	return _connectedSessionCount;
}
