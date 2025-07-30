#include "types.h"

#pragma pack(push, 1)
struct PacketFrame {

	int32 totalFrameCount = 0;
	int32 frameIndex = 0;
};
#pragma pack(pop)