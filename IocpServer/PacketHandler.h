#pragma once
#include <functional>
#include "PacketHeader.h"
#include "PacketFrame.h"
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
	PKT_CS_ENTER_ROOM_COMPLETE = 1204,

	PKT_CS_SKILL_CAST = 1300,



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
	PKT_SC_PLAYER_LEAVE_GRID_NOTIFICATION = 2109,
	PKT_SC_PLAYER_ENTER_GRID_NOTIFICATION = 2110,

	PKT_SC_SKILL_RESULT = 2200,
	PKT_SC_SKILL_CAST_NOTIFICATION = 2201,

};

static function<void(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service)> packetHandleArray[UINT16_MAX];

class PacketHandler{
public:
	static void RegisterPacketHandlers();
public:
	static void HandlePacket(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);

	template<typename T>
	static shared_ptr<Buffer> MakeSendBuffer(const T& packet, PacketId packetId);

private:
	static void Handle_Invalid(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);

	/*------------
		C -> S
	-------------*/
	// static void Handle_CS_(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service);
	static void Handle_CS_Ping(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_CS_Login_Request(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_CS_Room_List_Request(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_CS_Player_Info_Request(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_CS_Room_Player_List_Request(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_CS_Create_Room_Request(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_CS_Enter_Room_Request(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_CS_Player_Move_Request(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_CS_Enter_Room_Complete(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_CS_Skill_Cast(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);

	/*------------
		S -> C
	-------------*/
	static void Handle_CS_Pong(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_SC_Login_Response(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_SC_Room_List_Response(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_SC_Player_Info_Response(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_SC_Player_List_Response(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_SC_Enter_Room_Response(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* service);
	static void Handle_SC_Create_Room_Response(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* serviec);
	static void Handle_SC_Player_Enter_Room_Notification(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* serviec);
	static void Handle_SC_Player_Move_Notification(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* serviec);
	static void Handle_SC_Player_List_In_Grid(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* serviec);
	static void Handle_SC_Skill_Result(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* serviec);
	static void Handle_SC_Skill_Cast_Notification(shared_ptr<GameSession> session, const PacketHeader::View& packetHeaderView, Service* serviec);
};

template<typename T>
shared_ptr<Buffer> PacketHandler::MakeSendBuffer(const T& packet, PacketId packetId) {

	shared_ptr<Buffer> sendBuffer = nullptr;

	string serializedData;
	if (!packet.SerializeToString(&serializedData)) {
		spdlog::error("Failed to serialize packetId: {}", (int32)packetId);
		return sendBuffer; // 빈 벡터 반환
	}

	const int32 headerSize = sizeof(PacketHeader);
	const int32 dataSize = static_cast<int32>(serializedData.size());

	sendBuffer = shared_ptr<Buffer>(LSendBufferPool->Pop(headerSize + dataSize), [](Buffer* buffer) { buffer->ReturnToOwner(); });

	if (sendBuffer->WriteSize() != 0) {
		spdlog::info("[PacketHandler::MakeSendBuffer] SendBuffer Write Size Not Zero - BufferWriteSize {}, ", sendBuffer->WriteSize());
		return sendBuffer; // 빈 벡터 반환		
	}
	if (static_cast<int32>(sendBuffer->Capacity()) < headerSize + dataSize) {
		spdlog::error("Buffer capacity ({}) too small for packetId {} with data size {}", sendBuffer->Capacity(), (int32)packetId, dataSize);
		return sendBuffer; // 빈 벡터 반환
	}

	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->GetBuffer());
	header->packetId = htonl(packetId);
	header->packetSize = htonl(headerSize + dataSize);

	memcpy(header + 1, serializedData.data(), dataSize);

	return sendBuffer;
}
