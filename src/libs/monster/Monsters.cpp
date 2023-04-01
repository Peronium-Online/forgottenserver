#include "libs/monster/Monsters.h"

#include "../../configmanager.h"
#include "libs/monster/Monster.h"

#include <boost/algorithm/string.hpp>

extern ConfigManager g_config;

bool Monsters::load(pugi::xml_node node, bool reloading)
{
	std::string name = boost::algorithm::to_lower_copy<std::string>(node.attribute("name").as_string());
	std::string file = "data/monster/" + std::string(node.attribute("file").as_string());

	bool forceLoad = g_config.getBoolean(ConfigManager::FORCE_MONSTERTYPE_LOAD);

	if ((forceLoad || reloading) && monsters.find(name) != monsters.end()) {
		auto monster = new Monster(file, "monster");
		if (!monster->loadFromXML(reloading)) {
			std::cout << "[Error - Monsters::load] Failed to load monster: " << file << std::endl;
		}
	}

	return true;
}
