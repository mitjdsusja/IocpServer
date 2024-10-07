#pragma once
#include <functional>

enum PacketId {

	NONE = 1,
	
	PKT_C = 1000,
	PKT_C_REQUEST_USER_INFO = 1001,
	PKT_C_REQUEST_OTHER_USER_INFO = 1002,
	PKT_C_SEND_POS = 1003,

	PKT_S = 2000,
	PKT_S_RESPONSE_USER_INFO = 2001,
	PKT_S_RESPONSE_OTHER_USER_INFO = 2002,
	PKT_S_BROADCAST_POS = 2003,
	PKT_S_ADD_USER = 2004,
};

#pragma pack(push, 1)
struct PacketHeader {

	int32 packetSize = 0;
	int32 packetId = 0;
};

struct Packet_C_Request_User_Info{

	int32 packetSize = 0;
	int32 packetId = PKT_C_REQUEST_USER_INFO;
};
struct Packet_C_Request_Other_User_Info {

	// TODO : 가변 데이터 처리
	int32 packetSize = 0;
	int32 packetId = 0;
};
struct Packet_C_Send_Pos {
	int32 packetSize = 0;
	int32 packetId = PKT_C_SEND_POS;

	int32 playerId = 0;
	float posX = 0;
	float posY = 0;
	float posZ = 0;
};

struct Packet_S_Response_User_Info {
	int32 packetSize = 0;
	int32 packetId = PKT_S_RESPONSE_USER_INFO;

	int32 playerId = 0;
};
struct Packet_S_Response_Other_User_Info {
	int32 packetSize = 0;
	int32 packetId = PKT_S_RESPONSE_OTHER_USER_INFO;

	int32 playerCount = 0;

	void AppendUserIdData(BYTE* packet, int32 userId) {
		packet += sizeof(int32) * (3 + playerCount);
		memcpy(packet, &userId, sizeof(int32));
		playerCount++;
	}
};
struct Packet_S_Broadcast_Pos {
	int32 packetSize = 0;
	int32 packetId = PKT_S_BROADCAST_POS;

	int32 playerId = 0;
	float posX = 0;
	float posY = 0;
	float posZ = 0;
};
struct Packet_S_Add_User {
	int32 packetSize = 0;
	int32 packetId = PKT_S_ADD_USER;

	int32 playerId = 0;
};
#pragma pack(pop)

static function<void(shared_ptr<Session> session, PacketHeader* packet, Service* service)> packetHandleArray[UINT16_MAX];

class PacketHandler{
public:
	static void Init();
public:
	static void HandlePacket(shared_ptr<Session> session, PacketHeader* buffer, Service* service);

private:
	static void Handle_Invalid(shared_ptr<Session> session, PacketHeader* buffer, Service* service);

	static void Handle_C_Request_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void Handle_C_Request_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void Handle_C_Send_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service);

	static void Handle_S_Response_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void Handle_S_Response_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void Handle_S_Broadcast_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void Handle_S_Add_User(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
};



