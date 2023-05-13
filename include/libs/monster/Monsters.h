
#ifndef PR_MONSTERS_H
#define PR_MONSTERS_H

#include "../../../src/luascript.h"
#include "libs/monster/MonsterType.h"
#include "libs/util/xml/XMLLoadable.h"

#include <map>

class Monsters final : virtual public XMLLoadable
{
private:
	std::map<std::string, MonsterType> monsterTypes;
	std::unique_ptr<LuaScriptInterface> scriptInterface;

	virtual bool load(pugi::xml_node node, bool reloading) override;

	void initializeScriptInterface()
	{
		this->scriptInterface.reset(new LuaScriptInterface("Monster Interface"));
		this->scriptInterface->initState();
	}

public:
	Monsters()
	{
		this->filepath = "data/monster/monsters.xml";
		this->childNode = "monsters";
	};
	Monsters(const Monsters&) = delete;

	Monsters& operator=(const Monsters&) = delete;

	virtual MonsterType* findMonsterTypeByName(std::string name);

	virtual bool reload();

	virtual void setMonsterTypeScript(MonsterType& monsterType, std::string script);
	void addMonsterType(MonsterType& monsterType)
	{
		this->monsterTypes.emplace(monsterType.name, std::move(monsterType));
	}
};

Monsters g_monsters;

#endif
