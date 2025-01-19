#pragma once
#include <functional>
#include "PacketHeader.h"

enum PacketId {

	NONE = 1,

	PKT_CS = 1000,
	PKT_CS_CONNECT_SERVER = 1001,
	PKT_CS_REQUEST_SERVER_STATE = 1002,
	PKT_CS_MOVE_USER = 1003,

	PKT_SC = 2000,
	PKT_SC_ACCEPT_CLIENT = 2001,
	PKT_SC_RESPONSE_SERVER_STATE = 2002,
	PKT_SC_RESULT_MOVE_USER = 2003,
	PKT_SC_CONNET_OTHER_USER = 2004,
	PKT_SC_BROADCAST_USER_INFO = 2005,
};

static function<void(shared_ptr<Session> session, shared_ptr<Buffer> packet, Service* service)> packetHandleArray[UINT16_MAX];

class PacketHandler {
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

