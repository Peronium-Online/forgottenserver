#ifndef PR_MONSTER_H
#define PR_MONSTER_H

#include "creature.h"
#include "libs/monster/MonsterType.h"
#include "libs/util/xml/XMLLoadable.h"

uint32_t MONSTER_AUTO_ID = 0x21000000;

class Monster final : public Creature
{
private:
	MonsterType* mType;

public:
	Monster() = default;
	explicit Monster(MonsterType* mType);
	~Monster();

	// non-copyable
	Monster(const Monster&) = delete;
	Monster& operator=(const Monster&) = delete;

	static Monster* createMonsterByName(const std::string& name);

	CreatureType_t getType() const override { return CREATURETYPE_MONSTER; }

	std::string getDescription(int32_t) const override;
	const std::string& getName() const override;
	const std::string& getNameDescription() const override;

	void setID() override;

	void addList() override;
	void removeList() override;
};

#endif
