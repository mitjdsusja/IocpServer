#pragma once
#include <functional>

enum PacketId {

	NONE = 1,
	
	PKT_CS = 1000,
	PKT_CS_REQUEST_USER_INFO = 1001,
	PKT_CS_REQUEST_OTHER_USER_INFO = 1002,
	PKT_CS_SEND_POS = 1003,

	PKT_SC = 2000,
	PKT_SC_RESPONSE_USER_INFO = 2001,
	PKT_SC_RESPONSE_OTHER_USER_INFO = 2002,
	PKT_SC_BROADCAST_POS = 2003,
	PKT_SC_ADD_USER = 2004,
};

#pragma pack(push, 1)
struct PacketHeader {

	int32 packetSize = 0;
	int32 packetId = 0;
};
//
//struct Packet_C_Request_User_Info{
//
//	int32 packetSize = 0;
//	int32 packetId = PKT_CS_REQUEST_USER_INFO;
//};
//struct Packet_C_Request_Other_User_Info {
//
//	// TODO : 가변 데이터 처리
//	int32 packetSize = 0;
//	int32 packetId = 0;
//};
//struct Packet_C_Send_Pos {
//	int32 packetSize = 0;
//	int32 packetId = PKT_CS_SEND_POS;
//
//	int32 playerId = 0;
//	float posX = 0;
//	float posY = 0;
//	float posZ = 0;
//};
//
//struct Packet_S_Response_User_Info {
//	int32 packetSize = 0;
//	int32 packetId = PKT_SC_RESPONSE_USER_INFO;
//
//	int32 playerId = 0;
//};
//struct Packet_S_Response_Other_User_Info {
//	int32 packetSize = 0;
//	int32 packetId = PKT_SC_RESPONSE_OTHER_USER_INFO;
//
//	int32 playerCount = 0;
//
//	void AppendUserIdData(BYTE* packet, int32 userId) {
//		packet += sizeof(int32) * (3 + playerCount);
//		memcpy(packet, &userId, sizeof(int32));
//		playerCount++;
//	}
//};
//struct Packet_S_Broadcast_Pos {
//	int32 packetSize = 0;
//	int32 packetId = PKT_SC_BROADCAST_POS;
//
//	int32 playerId = 0;
//	float posX = 0;
//	float posY = 0;
//	float posZ = 0;
//};
//struct Packet_S_Add_User {
//	int32 packetSize = 0;
//	int32 packetId = PKT_SC_ADD_USER;
//
//	int32 playerId = 0;
//};
#pragma pack(pop)

static function<void(shared_ptr<Session> session, PacketHeader* packet, Service* service)> packetHandleArray[UINT16_MAX];

class PacketHandler{
public:
	static void Init();
public:
	static void HandlePacket(shared_ptr<Session> session, PacketHeader* buffer, Service* service);

	template<typename T>
	static SendBuffer* MakeSendBuffer(T& packet, PacketId packetId);

private:
	static void Handle_Invalid(shared_ptr<Session> session, PacketHeader* buffer, Service* service);

	static void Handle_CS_Request_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void Handle_CS_Request_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void Handle_CS_Send_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service);

	static void Handle_SC_Response_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void Handle_SC_Response_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void Handle_SC_Broadcast_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void Handle_SC_Add_User(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
};

template<typename T>
SendBuffer* PacketHandler::MakeSendBuffer(T& packet, PacketId packetId){
	SendBuffer* sendBuffer = LSendBufferPool->Pop();
	PacketHeader* header = (PacketHeader*)sendBuffer->Buffer();

	int32 dataSize = (int32)packet.ByteSizeLong();
	int32 packetSize = sizeof(PacketHeader) + dataSize;

	header->packetId = packetId;
	header->packetSize = packetSize;

	packet.SerializeToArray(&header[1], dataSize);

	sendBuffer->Write(packetSize);
	return sendBuffer;
}
