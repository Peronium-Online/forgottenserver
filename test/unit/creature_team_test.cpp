#include "../../src/creature.h"
#include "../../src/monster.h"
#include "../../src/player.h"

#include <gtest/gtest.h>

extern Monsters g_monsters;

TEST(CreatureTeamTest, DoNotAttackSameTeam)
{
	auto player = new Player(nullptr);
	player->setTeam(1);

	MonsterType* mType = g_monsters.getMonsterType("dragon");
	auto monster = new Monster(mType);
	monster->setTeam(1);

	player->setAttackedCreature(monster);
	monster->setAttackedCreature(player);

	EXPECT_FALSE(player->getAttackedCreature());
	EXPECT_FALSE(monster->getAttackedCreature());
}
