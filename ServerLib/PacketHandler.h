#pragma once
#include <functional>

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

#pragma pack(push, 1)
struct PacketHeader {

	int32 packetSize = 0;
	int32 packetId = 0;

	int32 GetDataSize() {
		return packetSize - sizeof(PacketHeader);
	}
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

static function<void(shared_ptr<Session> session, shared_ptr<Buffer> packet, Service* service)> packetHandleArray[UINT16_MAX];

class PacketHandler{
public:
	static void Init();
public:
	static void HandlePacket(shared_ptr<Session> session, PacketHeader* dataBuffer, Service* service);

	template<typename T>
	static Buffer* MakeSendBuffer(T& packet, PacketId packetId);

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
Buffer* PacketHandler::MakeSendBuffer(T& packet, PacketId packetId){
	Buffer* sendBuffer = LSendBufferPool->Pop();
	PacketHeader* header = (PacketHeader*)sendBuffer->GetBuffer();

	int32 dataSize = (int32)packet.ByteSizeLong();
	int32 packetSize = sizeof(PacketHeader) + dataSize;

	header->packetId = htonl(packetId);
	header->packetSize = htonl(packetSize);

	packet.SerializeToArray(&header[1], dataSize);

	sendBuffer->Write(packetSize);
	return sendBuffer;
}
