#pragma once
#include <functional>
#include "PacketHeader.h"
#include "GameSession.h"
#include "spdlog/spdlog.h"

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
	static void Handle_SC_Create_Room_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec);
	static void Handle_SC_Player_Enter_Room_Notification(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec);
	static void Handle_SC_Player_Move_Notification(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec);
	static void Handle_SC_Player_List_In_Grid(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec);
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
	if (!packet.SerializeToString(&serializedData)) {
		spdlog::error("Failed to serialize packetId: {}", (int32)packetId);
		return sendBuffers; // 빈 벡터 반환
	}

	const int32 headerSize = sizeof(PacketHeader);
	const int32 frameSize = sizeof(PacketFrame);

	int32 totalDataSize = static_cast<int32>(serializedData.size());
	int32 offset = 0;
	int32 frameCount = 0;

	// 최소 한 프레임은 만들어야 하므로, totalDataSize == 0도 처리
	do {
		shared_ptr<Buffer> sendBuffer = shared_ptr<Buffer>(LSendBufferPool->Pop(), [](Buffer* buffer) {LSendBufferPool->Push(buffer); });

		const int32 bufferCapacity = sendBuffer->Capacity();

		// 버퍼 크기가 헤더+프레임 크기보다 작으면
		if (bufferCapacity <= headerSize + frameSize) {
			spdlog::error("Buffer capacity ({}) too small for headers", bufferCapacity);
			break;
		}

		const int32 maxPayloadSize = bufferCapacity - headerSize - frameSize;

		// payloadSize는 남은 데이터 크기와 maxPayloadSize 중 작은 값
		int32 payloadSize = std::min(maxPayloadSize, totalDataSize - offset);

		// payloadSize가 음수면 종료
		if (payloadSize < 0) {
			spdlog::error("Negative payload size calculated: {}", payloadSize);
			break;
		}

		int32 packetSize = headerSize + frameSize + payloadSize;

		// packetSize가 버퍼 용량 초과하지 않는지 검사
		if (packetSize > bufferCapacity) {
			spdlog::error("Packet size ({}) exceeds buffer capacity ({})", packetSize, bufferCapacity);
			break;
		}

		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->GetBuffer());
		header->packetId = htonl(packetId);
		header->packetSize = htonl(packetSize);

		PacketFrame* frame = reinterpret_cast<PacketFrame*>((BYTE*)header + headerSize);
		frame->packetId = htonl(packetId);
		frame->frameIndex = htonl(frameCount);

		BYTE* payloadPtr = reinterpret_cast<BYTE*>(frame) + frameSize;
		if (payloadSize > 0)
			memcpy(payloadPtr, serializedData.data() + offset, payloadSize);

		sendBuffer->Write(packetSize);
		sendBuffers.push_back(sendBuffer);

		offset += payloadSize;
		frameCount++;

	} while (offset < totalDataSize || frameCount == 0);

	for (auto& buffer : sendBuffers) {
		PacketFrame* frame = reinterpret_cast<PacketFrame*>(buffer->GetBuffer() + sizeof(PacketHeader));
		frame->totalFrameCount = htonl(frameCount);
	}

	return sendBuffers;
}
