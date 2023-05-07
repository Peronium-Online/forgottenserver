#include "libs/monster/Monsters.h"

#include "../../configmanager.h"
#include "libs/monster/MonsterType.h"

#include <boost/algorithm/string.hpp>

extern ConfigManager g_config;

MonsterType* Monsters::findMonsterTypeByName(std::string name)
{
	auto it = this->monsterTypes.find(boost::algorithm::to_lower_copy(name));
	if (it != this->monsterTypes.end()) {
		return &it->second;
	}
}

bool Monsters::load(pugi::xml_node node, bool reloading)
{
	std::string name = boost::algorithm::to_lower_copy<std::string>(node.attribute("name").as_string());
	std::string file = "data/monster/" + std::string(node.attribute("file").as_string());

	bool forceLoad = g_config.getBoolean(ConfigManager::FORCE_MONSTERTYPE_LOAD);

	if (forceLoad || reloading) {
		auto mTypeBuilder = new MonsterType::Builder(file);
		if (mTypeBuilder->loadFromXML(reloading)) {
			this->addMonsterType(mTypeBuilder->build());
		} else {
			std::cout << "[Error - Monsters::load] Failed to load monster: " << file << std::endl;
		}
	}

	return true;
}
