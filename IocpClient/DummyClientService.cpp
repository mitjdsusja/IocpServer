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

	int count = 1;
	wcout << _sessions.size() << endl;
	for (auto& p : _sessions) {

		shared_ptr<Session>& session = p.second;

		auto& player = p.second;
		sendPacketLoginrequest.set_id("bot" + to_string(count));
		sendPacketLoginrequest.set_password("bot" + to_string(count));

		++count;

		const vector<shared_ptr<Buffer>> sendBuffer = PacketHandler::MakeSendBuffer(sendPacketLoginrequest, PacketId::PKT_CS_LOGIN_REQUEST);

		for (auto& buffer : sendBuffer) {

			session->Send(buffer);
		}
	}
}

void DummyClientService::AddConnectedSessionCount(){

	_connectedSessionCount.fetch_add(1);
}

uint32 DummyClientService::GetConnectedSessionCount(){

	return _connectedSessionCount;
}
