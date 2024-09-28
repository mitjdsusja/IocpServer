#pragma once
#include <functional>

enum PacketId {

	NONE = 1,
	
	PKT_C_REQUEST_INFO = 1001,
	PKT_C_POS = 1002,

	PKT_S_RESPONSE_INFO = 2001,
	PKT_S_BROADCAST_POS = 2002,
	PKT_S_ADDPLAYER = 2003,
};

#pragma pack(push, 1)
struct PacketHeader {

	int32 packetSize = 0;
	int32 packetId = 0;
};

struct Packet_C_Request_Info{

	int32 packetSize = 0;
	int32 packetId = PKT_C_REQUEST_INFO;
};

struct Packet_C_Pos {
	int32 packetSize = 0;
	int32 packetId = PKT_C_POS;

	int32 playerId = 0;
	float posX = 0;
	float posY = 0;
	float posZ = 0;
};

struct Packet_S_Response_Info {
	int32 packetSize = 0;
	int32 packetId = PKT_C_REQUEST_INFO;

	int32 playerId = 0;
};

struct Packet_S_Broadcast_Pos {
	int32 packetSize = 0;
	int32 packetId = PKT_S_BROADCAST_POS;

	int32 playerId = 0;
	float posX = 0;
	float posY = 0;
	float posZ = 0;
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

	static void Handle_C_Request_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void Handle_C_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service);

	static void Handle_S_Response_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service);
	static void handle_S_Pos_Broadcast(shared_ptr<Session> session, PacketHeader* buffer, Service* service);

};



