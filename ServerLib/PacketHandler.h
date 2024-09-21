#pragma once

enum PacketId {

	DEFAULT = 1,
	
	C_SEND = 1001,
	C_MOVE = 1002,

	S_SEND = 2001,
	S_MOVE = 2002,
};

#pragma pack(push, 1)
struct PacketHeader {

	int32 packetSize = 0;
	int32 packetId = 0;
};

struct Packet_C_SEND{

	int32 packetSize = 0;
	int32 packetId = C_SEND;

	int16 id = 0;
	int32 hp = 0;
	int64 mp = 0;
};

struct Packet_C_MOVE {
	int32 packetSize = 0;
	int32 packetId = C_MOVE;

	int32 playerId = 0;
	float posX = 0;
	float posY = 0;
	float posZ = 0;
};

struct Packet_S_MOVE {
	int32 packetSize = 0;
	int32 packetId = C_MOVE;

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
	static void Handle_S_SEND(PacketHeader* buffer, Service* service);
	static void Handle_C_SEND(PacketHeader* buffer, Service* service);
	static void Handle_C_MOVE(PacketHeader* buffer, Service* service);
};



