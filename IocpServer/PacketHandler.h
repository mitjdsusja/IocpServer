#pragma once
#include <functional>
#include "PacketHeader.h"
#include "GameSession.h"

enum PacketId {

	/*------------
		C -> S
	-------------*/
	// 1000 ~ 1099
	PKT_CS_1000 = 1000,
	PKT_CS_LOGIN_REQUEST = 1001,
	PKT_CS_PING = 1002,

	// 1100 ~ 1199
	PKT_CS_ROOM_LIST_REQUEST = 1101,
	PKT_CS_MY_PLAYER_INFO_REQUEST = 1102,
	PKT_CS_ROOM_PLAYER_LIST_REQUEST = 1103,

	// 1200 ~ 
	PKT_CS_1200 = 1200,
	PKT_CS_ENTER_ROOM_REQUEST = 1201,
	PKT_CS_CREATE_ROOM_REQUEST = 1202,
	PKT_CS_PLAYER_MOVE_REQUEST = 1203,


	/*------------
		S -> C
	-------------*/
	// 2000 ~ 
	PKT_SC_2000 = 2000,
	PKT_SC_LOGIN_RESPONSE = 2001,
	PKT_SC_PONG = 2002,

	// 2100 ~ 2199
	PKT_SC_2100 = 2100,
	PKT_SC_ROOM_LIST_RESPONSE = 2101,
	PKT_SC_MY_PLAYER_INFO_RESPONSE = 2102,
	PKT_SC_ROOM_PLAYER_LIST_RESPONSE = 2103,
	PKT_SC_ENTER_ROOM_RESPONSE = 2104,
	PKT_SC_CREATE_ROOM_RESPONSE = 2105,
	PKT_SC_PLAYER_ENTER_ROOM_NOTIFICATION = 2106,
	PKT_SC_PLAYER_MOVE_NOTIFICATION = 2107,
	PKT_SC_PLAYER_LIST_IN_GRID = 2108,

};

static function<void(shared_ptr<GameSession> session, shared_ptr<Buffer> packet, Service* service)> packetHandleArray[UINT16_MAX];

class PacketHandler{
public:
	static void RegisterPacketHandlers();
public:
	static void HandlePacket(shared_ptr<GameSession> session, PacketHeader* dataBuffer, Service* service);

	template<typename T>
	static vector<shared_ptr<Buffer>> MakeSendBuffer(const T& packet, PacketId packetId);

private:
	static void Handle_Invalid(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);

	/*------------
		C -> S
	-------------*/
	// static void Handle_CS_(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_CS_Ping(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_CS_Login_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_CS_Room_List_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_CS_Player_Info_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_CS_Room_Player_List_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_CS_Create_Room_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_CS_Enter_Room_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_CS_Player_Move_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);

	/*------------
		S -> C
	-------------*/
	static void Handle_CS_Pong(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_SC_Login_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_SC_Room_List_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_SC_Player_Info_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_SC_Player_List_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_SC_Enter_Room_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service);
};

//template<typename T>
//shared_ptr<Buffer> PacketHandler::MakeSendBuffer(T& packet, PacketId packetId) {
//	shared_ptr<Buffer> sendBuffer = shared_ptr<Buffer>(GSendBufferPool->Pop(), [](Buffer* buffer) { GSendBufferPool->Push(buffer); });
//	PacketHeader* header = (PacketHeader*)sendBuffer->GetBuffer();
//
//	int32 dataSize = (int32)packet.ByteSizeLong();
//	int32 packetSize = sizeof(PacketHeader) + dataSize;
//
//	header->packetId = htonl(packetId);
//	header->packetSize = htonl(packetSize);
//
//	packet.SerializeToArray(((BYTE*)header + sizeof(PacketHeader)), dataSize);
//
//	sendBuffer->Write(packetSize);
//	return sendBuffer;
//}

template<typename T>
vector<shared_ptr<Buffer>> PacketHandler::MakeSendBuffer(const T& packet, PacketId packetId) {

	vector<shared_ptr<Buffer>> sendBuffers;

	string serializedData;
	packet.SerializeToString(&serializedData);

	const int32 headerSize = sizeof(PacketHeader);
	const int32 frameSize = sizeof(PacketFrame);
	
	int32 totalDataSize = serializedData.size();
	int32 offset = 0;
	int32 frameCount = 0;

	for(uint32 frameIndex = 0;  offset < totalDataSize; ++frameIndex) {

		shared_ptr<Buffer> sendBuffer = shared_ptr<Buffer>(GSendBufferPool->Pop(), [](Buffer* buffer) { GSendBufferPool->Push(buffer); });

		const int32 bufferCapacity = sendBuffer->Capacity();
		const int32 maxPayloadSize = bufferCapacity - headerSize - frameSize;

		int32 payloadSize = min(maxPayloadSize, totalDataSize - offset);
		int32 packetSize = headerSize + frameSize + payloadSize;

		PacketHeader* header = (PacketHeader*)sendBuffer->GetBuffer();
		header->packetId = htonl(packetId);
		header->packetSize = htonl(packetSize);
		
		PacketFrame* frame = (PacketFrame*)((BYTE*)header + sizeof(PacketHeader));
		frame->packetId = htonl(packetId);
		frame->frameIndex = htonl(frameIndex);

		BYTE* payloadPtr = (BYTE*)frame + sizeof(PacketFrame);
		memcpy(payloadPtr, serializedData.data() + offset, payloadSize);

		sendBuffer->Write(packetSize);
		sendBuffers.push_back(sendBuffer);

		offset += payloadSize;
		frameCount++;
	}

	for (auto& buffer : sendBuffers) {
		PacketFrame* frame = (PacketFrame*)(buffer->GetBuffer() + sizeof(PacketHeader));
		frame->totalFrameCount = htonl(frameCount);
	}

	return sendBuffers;
}