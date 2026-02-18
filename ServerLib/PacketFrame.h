#include "types.h"

#pragma pack(push, 1)
struct PacketFrame {

	int32 packetId = 0;
	int32 totalFrameCount = 0;
	int32 frameIndex = 0;

	struct View {

		int32 frameId = 0;
		int32 totalFrameCount = 0;
		int32 frameIndex = 0;
		const PacketFrame* frame = nullptr;
		BYTE* payload = nullptr;
		int32 payloadSize = 0;
	};

	static bool TryParse(BYTE* buffer, int32 dataSize, View& out) {

		if (dataSize < sizeof(PacketFrame)) {
			return false;
		}

		PacketFrame* frame = reinterpret_cast<PacketFrame*>(buffer);

		const int32 frameId = static_cast<int32>(ntohl(frame->packetId));
		const int32 totalFrameCount = static_cast<int32>(ntohl(frame->totalFrameCount));
		const int32 frameIndex = static_cast<int32>(ntohl(frame->frameIndex));

		if (frameId < 0) {
			return false;
		}
		if (totalFrameCount <= 0) {
			return false;
		}
		if (frameIndex < 0 || frameIndex >= totalFrameCount) {
			return false;
		}

		BYTE* payload = buffer + sizeof(PacketFrame);
		int32 payloadSize = dataSize - static_cast<int32>(sizeof(PacketFrame));
		if (payloadSize < 0) {
			return false;
		}

		out.frameId = frameId;
		out.totalFrameCount = totalFrameCount;
		out.frameIndex = frameIndex;
		out.frame = frame;
		out.payload = payload;
		out.payloadSize = payloadSize;

		return true;
	}
};
#pragma pack(pop)