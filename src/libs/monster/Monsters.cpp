#include "libs/monster/Monsters.h"

#include "../../configmanager.h"
#include "libs/monster/MonsterType.h"

#include <boost/algorithm/string.hpp>

extern ConfigManager g_config;
Monsters g_monsters;

MonsterType* Monsters::findMonsterTypeByName(std::string name)
{
	auto it = this->monsterTypes.find(boost::algorithm::to_lower_copy(name));
	if (it != this->monsterTypes.end()) {
		return it->second;
	}

	return &MonsterType::UNDEFINED_MONSTER_TYPE;
}

bool Monsters::load(pugi::xml_node node, bool reloading)
{
	std::string name = boost::algorithm::to_lower_copy<std::string>(node.attribute("name").as_string());
	std::string file = "data/monster/" + std::string(node.attribute("file").as_string());

	bool forceLoad = g_config.getBoolean(ConfigManager::FORCE_MONSTERTYPE_LOAD);

	if (forceLoad || reloading) {
		auto mTypeBuilder = new MonsterType::Builder(file);
		if (mTypeBuilder->loadFromXML(reloading)) {
			auto mType = mTypeBuilder->build();
			this->addMonsterType(name, mType);
		} else {
			std::cout << "[Error - Monsters::load] Failed to load monster: " << file << std::endl;
		}
	}

	return true;
}

bool Monsters::reload()
{
	loaded = false;

	scriptInterface.reset();

	return loadFromXML(true);
}

void Monsters::setMonsterTypeScript(MonsterType& mType, std::string filename)
{
	if (!this->scriptInterface) {
		this->initializeScriptInterface();
	}

	if (this->scriptInterface->loadFile("data/monster/scripts/" + filename) == 0) {
		mType.info.scriptInterface = this->scriptInterface.get();
		mType.info.creatureAppearEvent = this->scriptInterface->getEvent("onCreatureAppear");
		mType.info.creatureDisappearEvent = this->scriptInterface->getEvent("onCreatureDisappear");
		mType.info.creatureMoveEvent = this->scriptInterface->getEvent("onCreatureMove");
		mType.info.creatureSayEvent = this->scriptInterface->getEvent("onCreatureSay");
		mType.info.thinkEvent = this->scriptInterface->getEvent("onThink");
	} else {
		std::cout << "[Warning - MonsterType::setScript] Can not load script: " << filename << std::endl;
		std::cout << this->scriptInterface->getLastLuaError() << std::endl;
	}
}
