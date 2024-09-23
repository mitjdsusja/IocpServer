#pragma once

enum PacketId {

	DEFAULT = 1,
	
	c_send = 1001,
	c_pos = 1002,

	s_send = 2001,
	s_pos_broadcast = 2002,
};

#pragma pack(push, 1)
struct PacketHeader {

	int32 packetSize = 0;
	int32 packetId = 0;
};

struct Packet_C_SEND{

	int32 packetSize = 0;
	int32 packetId = c_send;
};

struct Packet_C_Pos {
	int32 packetSize = 0;
	int32 packetId = c_pos;

	int32 playerId = 0;
	float posX = 0;
	float posY = 0;
	float posZ = 0;
};

struct Packet_S_Pos_Broadcast {
	int32 packetSize = 0;
	int32 packetId = s_pos_broadcast;

	int32 playerId = 0;
	float posX = 0;
	float posY = 0;
	float posZ = 0;
};
#pragma pack(pop)

class PacketHandler{
public:
	static void HandlePacket(PacketHeader* buffer, Service* service);

private:
	static void Handle_C_SEND(PacketHeader* buffer, Service* service);
	static void Handle_C_Pos(PacketHeader* buffer, Service* service);

	static void Handle_S_SEND(PacketHeader* buffer, Service* service);
	static void handle_S_Pos_Broadcast(PacketHeader* buffer, Service* service);

};



