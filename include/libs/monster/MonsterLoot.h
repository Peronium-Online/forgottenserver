#ifndef PR_MONSTER_LOOT_H
#define PR_MONSTER_LOOT_H

#include "libs/util/xml/XMLElementBuilder.h"

#include <string>
#include <vector>

const uint32_t MAX_LOOTCHANCE = 100000;

class MonsterLoot : virtual public XMLElementBuilder<MonsterLoot*>
{
public:
	MonsterLoot()
	{
		itemId = 0;
		countMax = 1;
		chance = 0;
		subType = -1;
		actionId = -1;
	};

	uint16_t itemId;
	uint32_t countMax;
	uint32_t chance;

	// optional
	int32_t subType;
	int32_t actionId;
	std::string text;
	std::vector<MonsterLoot> childrenLoots;

	virtual MonsterLoot* loadFromXMLNode(pugi::xml_node node, bool reloading) override;

	virtual MonsterLoot* setItemId(uint16_t itemId);
	virtual MonsterLoot* setItemIdByName(std::string name);
	virtual MonsterLoot* setCountMax(uint32_t countMax);
	virtual MonsterLoot* setChance(uint32_t chance);
	virtual MonsterLoot* setSubType(int32_t subType);
	virtual MonsterLoot* setActionId(int32_t actionId);
	virtual MonsterLoot* setText(std::string text);
	virtual MonsterLoot* addChildLoot(MonsterLoot childLoot);
};

#endif
