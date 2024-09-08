#pragma once

enum PacketId {

	DEFAULT = 1,
	
	C_SEND = 1001,

	S_SEND = 2001,
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
#pragma pack(pop)

class PacketHandler{
public:
	static void HandlePacket(PacketHeader* buffer);

private:
	static void Handle_S_SEND(PacketHeader* buffer);
	static void Handle_C_SEND(PacketHeader* buffer);
};



