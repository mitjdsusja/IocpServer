#pragma once
#include "pch.h"
#include "RoomInfo.h"
#include "RoomPlayerData.h"

namespace RoomManagerResult {

	struct EnterRoomResult {

		enum FailReason {

			UNKNOWN = 0,
		};

		bool _success;
		FailReason _failReason;
		RoomInfo _roomInfo;
		RoomPlayerData _enterPlayerInfo;
		int32 _enterSessionId;
	};
}
