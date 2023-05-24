#ifndef PR_MONSTER_VOICE_H
#define PR_MONSTER_VOICE_H

#include <iostream>
#include <string>
#include <vector>

struct voiceBlock_t
{
	std::string text;
	bool yellText;
};

class MonsterVoice
{
private:
	uint32_t yellSpeedTicks;
	uint32_t yellChance;
	std::vector<voiceBlock_t> voiceBlocks;

public:
	MonsterVoice() = default;

	void setYellSpeedTicks(uint32_t ticks) { this->yellSpeedTicks = ticks; }

	void setYellChance(uint32_t chance)
	{
		if (chance > 100) {
			chance = 100;
			std::cout << "[Warning - Monsters::loadMonster] yell chance value out of bounds. " << std::endl;
		}

		this->yellChance = chance;
	}

	void addVoiceBlock(const std::string& text, bool yellText)
	{
		this->voiceBlocks.emplace_back(std::move(voiceBlock_t{text, yellText}));
	}

	void shrinkToFit() { this->voiceBlocks.shrink_to_fit(); }

	uint32_t numberOfSentences() { return this->voiceBlocks.size(); }

	void forEachVoice(const std::function<void(const std::string&, bool)>& callback)
	{
		for (const auto& voiceBlock : voiceBlocks) {
			callback(voiceBlock.text, voiceBlock.yellText);
		}
	}

	const voiceBlock_t* getRandomYell();
};

#endif
