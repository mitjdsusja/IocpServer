#pragma once
#include "pch.h"
#include "Vector.h"

enum SkillType {

	SKILL_TYPE_NONE = 0,
	SKILL_TYPE_PROJECTILE = 1,
	SKILL_TYPE_AOE = 2,
	SKILL_TYPE_TARGET = 3,
};;

struct SkillData {

	int32 skillId;
	int32 skillType;
	Vector<int16> startPos;
	Vector<int16> direction;
	Vector<int16> targetPos;
	int64 targetId;
	int64 casterId;
	int64 castTime;
};