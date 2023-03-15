#include "libs/monster/Monster.h"

#include "libs/monster/Monsters.h"

#include <boost/algorithm/string.hpp>

extern Monsters g_monsters;

bool Monster::load(pugi::xml_node node, bool reloading)
{
	MonsterType* mType = nullptr;

	pugi::xml_attribute attr;
	if (!(attr = node.attribute("name"))) {
		std::cout << "[Error - Monsters::loadMonster] Missing name in: " << this->filepath << std::endl;
		return false;
	}
	std::string lowerCasedMonsterName = boost::algorithm::to_lower_copy(attr.as_string());

	if (reloading) {
		auto it = g_monsters.findMonsterTypeByName(lowerCasedMonsterName);
		if (it != nullptr) {
			mType = it;
			mType->info = {};
		}
	}

	if (!mType) {
		mType = g_monsters.findMonsterTypeByName(lowerCasedMonsterName);
	}

	mType->name = attr.as_string();

	if ((attr = node.attribute("nameDescription"))) {
		mType->nameDescription = attr.as_string();
	} else {
		mType->nameDescription = "a " + lowerCasedMonsterName;
	}
}
