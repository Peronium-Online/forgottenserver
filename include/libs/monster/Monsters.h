
#ifndef PR_MONSTERS_H
#define PR_MONSTERS_H

#include "../../../src/luascript.h"
#include "libs/monster/MonsterType.h"
#include "libs/util/xml/XMLLoadable.h"

#include <map>

class Monsters final : virtual public XMLLoadable
{
private:
	std::map<std::string, MonsterType*> monsterTypes;
	std::unique_ptr<LuaScriptInterface> scriptInterface;

	bool load(pugi::xml_node node, bool reloading) override;

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
	void addMonsterType(MonsterType* monsterType)
	{
		const auto& name = boost::algorithm::to_lower_copy(monsterType->name);
		this->monsterTypes.emplace(name, std::move(monsterType));
	}

	void addMonsterType(const std::string name, MonsterType* monsterType)
	{
		this->monsterTypes.emplace(name, std::move(monsterType));
	}

	uint32_t monsterTypeCount() const { return this->monsterTypes.size(); }

	void forEachMonsterType(const std::function<void(MonsterType*)>& callback)
	{
		for (auto monsterType : this->monsterTypes) {
			callback(monsterType.second);
		}
	}
};

#endif
