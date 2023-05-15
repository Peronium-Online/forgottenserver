#ifndef PR_MONSTER_SEEK_H
#define PR_MONSTER_SEEK_H

#include <string>

class MonsterSeek
{
private:
	std::string name;
	uint32_t priority = 1;

public:
	MonsterSeek(std::string name) : name(name){};

	std::string getName() { return name; }

	void setPriority(uint32_t priority) { this->priority = priority; }
};

#endif
