#pragma once
#include <functional>

#pragma pack(push, 1)
struct PacketHeader {

	int32 packetSize = 0;
	int32 packetId = 0;

	int32 GetDataSize() {
		return packetSize - sizeof(PacketHeader);
	}
};
#pragma pack(pop)

static function<void(shared_ptr<Session> session, shared_ptr<Buffer> packet, Service* service)> packetHandleArray[UINT16_MAX];

class IPacketHandler{
public:
	static void Init();
public:
	static void HandlePacket(shared_ptr<Session> session, PacketHeader* dataBuffer, Service* service);

};
