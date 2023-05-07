#include "libs/monster/Monster.h"

#include "game.h"

extern Monsters g_monsters;
extern Game g_game;

Monster* Monster::createMonsterByName(const std::string& name)
{
	MonsterType* mType = g_monsters.findMonsterTypeByName(name);
	if (!mType) {
		return nullptr;
	}

	return new Monster(mType);
}

const std::string& Monster::getName() const { return mType->name; }

std::string Monster::getDescription(int32_t) const { return this->mType->nameDescription + '.'; }

const std::string& Monster::getNameDescription() const { return mType->nameDescription; }

void Monster::setID()
{
	if (id == 0) {
		id = MONSTER_AUTO_ID++;
	}
}

void Monster::addList() { g_game.addMonster(this); }

void Monster::removeList() { g_game.removeMonster(this); }
