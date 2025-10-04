#pragma once
#include "pch.h"
#include "RoomInfo.h"
#include "RoomPlayerData.h"
#include "SkillData.h"

namespace RoomResult {

	struct EnterRoomResult {

		enum FailReason {

			UNKNOWN = 0,
		};

		bool _success;
		FailReason _failReason;
		RoomInfo _roomInfo;
		RoomPlayerData _enterPlayerInfo;
		int32 _enterSessionId;
		vector<RoomPlayerData> _playerListInGrid;
	};

	struct SkillUseResult {
	
		bool _success;
		int16 _failReason;
		SkillData _skillData;
	};
}
