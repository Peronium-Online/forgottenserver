
#ifndef PR_MONSTERS_H
#define PR_MONSTERS_H

#include "../../../src/luascript.h"
#include "libs/monster/MonsterType.h"
#include "libs/util/xml/XMLLoadable.h"

#include <map>

class Monsters final : virtual public XMLLoadable
{
private:
	std::map<std::string, const MonsterType&> monsterTypes;
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

	virtual const MonsterType& findMonsterTypeByName(std::string name);

	virtual bool reload();

	virtual void setMonsterTypeScript(MonsterType& monsterType, std::string script);
	void addMonsterType(const MonsterType& monsterType)
	{
		this->monsterTypes.emplace(monsterType.name.substr(), std::move(monsterType));
	}

	uint32_t monsterTypeCount() const { return this->monsterTypes.size(); }

	void forEachMonsterType(const std::function<void(const MonsterType&)>& callback)
	{
		for (const auto& monsterType : this->monsterTypes) {
			callback(monsterType.second);
		}
	}
};

#endif
