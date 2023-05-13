#ifndef PR_MONSTER_H
#define PR_MONSTER_H

#include "../../../src/creature.h"
#include "libs/monster/MonsterType.h"
#include "libs/util/xml/XMLLoadable.h"

#include <unordered_set>

using CreatureHashSet = std::unordered_set<Creature*>;
using CreatureList = std::list<Creature*>;

uint32_t MONSTER_AUTO_ID = 0x21000000;

// TODO: find a better place for this enum
enum TargetSearchType_t
{
	TARGETSEARCH_DEFAULT,
	TARGETSEARCH_RANDOM,
	TARGETSEARCH_ATTACKRANGE,
	TARGETSEARCH_NEAREST,
};

class Monster final : public Creature
{
private:
	MonsterType* mType;
	std::string name;
	std::string nameDescription;

	Position masterPos;
	Spawn* spawn = nullptr;
	bool idle = false;
	bool walkingToSpawn = false;

	CreatureHashSet friendList;
	CreatureList targetList;

	int32_t challengeFocusDuration = 0;
	int32_t stepDuration = 0;

public:
	Monster() = default;
	explicit Monster(MonsterType* mType);
	~Monster();
	// non-copyable
	Monster(const Monster&) = delete;
	Monster& operator=(const Monster&) = delete;

	static Monster* createMonsterByName(const std::string& name);
	static int32_t despawnRange;
	static int32_t despawnRadius;

	bool isHostile() const { return mType->info.isHostile; }

	CreatureType_t getType() const override { return CREATURETYPE_MONSTER; }
	std::string getDescription(int32_t) const override;
	const std::string& getName() const override;
	const std::string& getNameDescription() const override;
	void setID() override;
	void addList() override;
	void removeList() override;

	void setName(const std::string& name);
	void setNameDescription(const std::string& nameDescription) { this->nameDescription = nameDescription; }
	std::string getName() { return this->name; }
	std::string getNameDescription() { return this->nameDescription; }
	MonsterType* getMonsterType() const { return mType; }

	const Position& getMasterPos() const { return masterPos; }
	void setMasterPos(Position pos) { masterPos = pos; }

	bool isInSpawnRange(const Position& pos) const;
	bool isWalkingToSpawn() const { return walkingToSpawn; }
	bool walkToSpawn();

	bool isIdle() const { return idle; }
	void setIdle(bool idle);
	void updateIdleStatus();

	void clearTargetList();
	void clearFriendList();

	bool isTarget(const Creature* creature) const;
	bool isSeekTarget(const Creature* creature) const;

	bool isFriend(const Creature* creature) const;
	void addFriend(Creature* creature);
	void removeFriend(Creature* creature);
	const CreatureHashSet& getFriendList() const { return friendList; }

	bool isOpponent(const Creature* creature) const;
	void addTarget(Creature* creature, bool pushFront = false);
	void removeTarget(Creature* creature);
	const CreatureList& getTargetList() const { return targetList; }
	bool searchTarget(TargetSearchType_t searchType = TARGETSEARCH_DEFAULT);
	bool selectTarget(Creature* creature);

	bool canUseAttack(const Position& pos, const Creature* target) const;

	void setSpawn(Spawn* spawn) { this->spawn = spawn; }

	bool isFleeing() const
	{
		return !isSummon() && getHealth() <= mType->info.runAwayHealth && challengeFocusDuration <= 0;
	}

	bool getDistanceStep(const Position& targetPos, Direction& direction, bool flee = false);
	bool getRandomStep(const Position& creaturePos, Direction& direction) const;
	bool canWalkTo(Position pos, Direction direction) const;
	bool canWalkOnFieldType(CombatType_t combatType) const;

	bool isTargetNearby() const { return stepDuration >= 1; }
};

#endif
