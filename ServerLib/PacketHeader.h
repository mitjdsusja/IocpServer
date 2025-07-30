#include "PacketFrame.h"

#pragma pack(push, 1)
struct PacketHeader {

	int32 packetSize = 0;
	int32 packetId = 0;

	int32 GetDataSize() {
		return packetSize - sizeof(PacketHeader);
	}
};
#pragma pack(pop)