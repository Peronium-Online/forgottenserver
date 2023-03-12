#include "libs/vocation/Vocation.h"

#include "../src/player.h"

#include <cmath>

static const uint32_t skillBase[SKILL_LAST + 1] = {50, 50, 50, 50, 30, 100, 20};

uint64_t Vocation::getReqSkillTries(uint8_t skill, uint16_t level)
{
	if (skill > SKILL_LAST) {
		return 0;
	}
	return skillBase[skill] *
	       std::pow(skillMultipliers[skill], static_cast<int32_t>(level - (MINIMUM_SKILL_LEVEL + 1)));
}

uint64_t Vocation::getReqMana(uint32_t magLevel)
{
	if (magLevel == 0) {
		return 0;
	}
	return 1600 * std::pow(manaMultiplier, static_cast<int32_t>(magLevel - 1));
}
