
#ifndef PR_MONSTERS_H
#define PR_MONSTERS_H

#include "../../../src/luascript.h"
#include "libs/monster/MonsterType.h"
#include "libs/util/xml/XMLLoadable.h"

#include <map>

class Monsters final : virtual public XMLLoadable
{
public:
	std::unique_ptr<LuaScriptInterface> scriptInterface;

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
