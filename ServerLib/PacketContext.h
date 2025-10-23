#pragma once
#include "ServerPch.h"
#include "PacketHeader.h"

#pragma pack(push, 1)
struct PacketContext {

	PacketHeader header;
	vector<BYTE> dataVector;

	const BYTE* GetDataPtr() const{

		if (dataVector.size() == 0) return nullptr;
		return dataVector.data();
	}
	int32 GetDataSize() const {

		return (int32)dataVector.size();
	}
};
#pragma pack(pop)