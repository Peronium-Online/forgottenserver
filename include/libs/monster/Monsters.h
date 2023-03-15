
#ifndef PR_MONSTERS_H
#define PR_MONSTERS_H

#include "libs/monster/MonsterType.h"
#include "libs/util/loader/XMLLoadable.h"

#include <map>

class Monsters final : public XMLLoadable
{
public:
	Monsters()
	{
		this->filepath = "data/monster/monsters.xml";
		this->childNode = "monsters";
	};
	Monsters(const Monsters&) = delete;
	Monsters& operator=(const Monsters&) = delete;

	virtual MonsterType* findMonsterTypeByName(std::string name);

private:
	std::map<std::string, MonsterType> monsters;

	virtual bool load(pugi::xml_node node, bool reloading) override;
};

#endif
