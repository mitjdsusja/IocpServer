#pragma once

#pragma pack(push, 1)
struct PacketHeader {

	int32 packetSize = 0;
	int32 packetId = 0;

	struct View {

		int32 id = 0;
		int32 size = 0;
		const PacketHeader* header = nullptr;
		BYTE* body = nullptr;
		int32 bodySize = 0;
	};

	static bool TryParse(BYTE* buffer, int32 dataSize, View& out) {

		if (dataSize < sizeof(PacketHeader)) {
			return false;
		}

		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

		const int32 id = static_cast<int32>(ntohl(header->packetId));
		const int32 size = static_cast<int32>(ntohl(header->packetSize));

		if(id < 0){
			return false;
		}
		if(size < static_cast<int32>(sizeof(PacketHeader))){
			return false;
		}
		if (dataSize < size) {
			return false;
		}

		out.id = id;
		out.size = size;
		out.header = header;
		out.body = buffer + sizeof(PacketHeader);
		out.bodySize = size - sizeof(PacketHeader);

		return true;
	}

	int32 GetDataSize() {
		return packetSize - sizeof(PacketHeader);
	}
};
#pragma pack(pop)