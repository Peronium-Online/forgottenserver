#ifndef PR_MONSTER_SUMMON_H
#define PR_MONSTER_SUMMON_H

#include <iostream>
#include <string>

const uint32_t MAX_SUMMONS = 100;

class MonsterSummon
{
private:
	int32_t chance = 100;
	int32_t speed = 1000;
	uint32_t max = 1;
	bool force = false;
	std::string name;

public:
	MonsterSummon(std::string name) : name(name) {}
	~MonsterSummon();

	void setChance(int32_t chance)
	{
		if (chance > 100) {
			chance = 100;
			std::cout << "[Warning - MonsterSummon::setChance] Summon chance value out of bounds. " << name
			          << std::endl;
		}
		this->chance = chance;
	}

	void setSpeed(int32_t speed) { this->speed = std::max(speed, 1); }

	void setMax(uint32_t max) { this->max = std::min(max, MAX_SUMMONS); }

	void setForce(bool force) { this->force = force; }
};

#endif
