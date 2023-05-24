#include "libs/monster/MonsterVoice.h"

#include "../../game.h"
#include "libs/util/tools/random.h"

extern Game g_game;

const voiceBlock_t* MonsterVoice::getRandomYell()
{
	if (yellSpeedTicks == 0) {
		return nullptr;
	}

	if (!voiceBlocks.empty() && (yellChance >= static_cast<uint32_t>(uniform_random(1, 100)))) {
		uint32_t index = uniform_random(0, voiceBlocks.size() - 1);
		const voiceBlock_t& vb = voiceBlocks[index];
		return &vb;
	}

	return nullptr;
}
