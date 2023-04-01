#include "libs/monster/Monster.h"

#include "libs/monster/Monsters.h"

#include <boost/algorithm/string.hpp>

extern Monsters g_monsters;

bool Monster::load(pugi::xml_node node, bool reloading)
{
	MonsterType* mType = MonsterType::Builder().loadFromXMLNode(node, reloading);

	return true;
}
