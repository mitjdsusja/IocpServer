#pragma once
#include <functional>
#include "PacketHeader.h"
#include "PacketHandler.h"
#include "Session.h"

enum PacketId {

	/*------------
		C -> S
	-------------*/
	// 1000 ~
	PKT_CS_1000 = 1000,
	PKT_CS_LOGIN = 1001,

	// Request 1100 ~ 1199
	PKT_CS_REQUEST_1100 = 1100,
	PKT_CS_REQUEST_ROOM_LIST = 1101,
	PKT_CS_REQUEST_USER_INFO = 1102,
	PKT_CS_REQUEST_USER_LIST = 1103,

	// 1200 ~ 
	PKT_CS_1200 = 1200,
	PKT_CS_ENTER_ROOM = 1201,


	/*------------
		S -> C
	-------------*/
	// 2000 ~ 
	PKT_SC_2000 = 2000,
	PKT_SC_LOGIN_SUCCESS = 2001,
	PKT_SC_LOGIN_FAIL = 2002,

	// Response 2100 ~ 2199
	PKT_SC_2100 = 2100,
	PKT_SC_RESPONSE_ROOM_LIST = 2101,
	PKT_SC_RESPONSE_USER_INFO = 2102,
	PKT_SC_RESPONSE_USER_LIST = 2103,

	// 2200 ~ 
	PKT_SC_2200 = 2200,
	PKT_SC_ENTER_ROOM_SUCCESS = 2201,
	PKT_SC_ENTER_ROOM_FAIL = 2202,

};

static function<void(shared_ptr<Session> session, shared_ptr<Buffer> packet, Service* service)> packetHandleArray[UINT16_MAX];

class PacketHandler{
public:
	static void RegisterPacketHandlers();
public:
	static void HandlePacket(shared_ptr<Session> session, PacketHeader* dataBuffer, Service* service);

	template<typename T>
	static shared_ptr<Buffer> MakeSendBuffer(T& packet, PacketId packetId);

private:
	static void Handle_Invalid(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service);

	static void Handle_CS_Connect_Server(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_CS_Request_Server_State(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_CS_Move_User(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service);

	static void Handle_SC_Accept_Client(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_SC_Response_Server_State(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_SC_Result_Move_User(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_SC_Connect_Other_User(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service);
};

template<typename T>
shared_ptr<Buffer> PacketHandler::MakeSendBuffer(T& packet, PacketId packetId) {
	shared_ptr<Buffer> sendBuffer = shared_ptr<Buffer>(GSendBufferPool->Pop(), [](Buffer* buffer) { GSendBufferPool->Push(buffer); });
	PacketHeader* header = (PacketHeader*)sendBuffer->GetBuffer();

	int32 dataSize = (int32)packet.ByteSizeLong();
	int32 packetSize = sizeof(PacketHeader) + dataSize;

	header->packetId = htonl(packetId);
	header->packetSize = htonl(packetSize);

	packet.SerializeToArray(((BYTE*)header + sizeof(PacketHeader)), dataSize);

	sendBuffer->Write(packetSize);
	return sendBuffer;
}

