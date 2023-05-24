#ifndef PR_MONSTER_H
#define PR_MONSTER_H

#include "../../../src/creature.h"
#include "libs/monster/MonsterType.h"
#include "libs/util/xml/XMLLoadable.h"

#include <unordered_set>

using CreatureHashSet = std::unordered_set<Creature*>;
using CreatureList = std::list<Creature*>;

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
	bool masterInRange = false;
	Spawn* spawn = nullptr;
	bool idle = false;
	bool walkingToSpawn = false;
	bool randomStepping = false;

	CreatureHashSet friendList;
	CreatureList targetList;

	uint32_t attackTicks = 0;
	uint32_t targetTicks = 0;
	uint32_t targetChangeTicks = 0;
	uint32_t defenseTicks = 0;
	uint32_t yellTicks = 0;

	int32_t minCombatValue = 0;
	int32_t maxCombatValue = 0;

	int32_t targetChangeCooldown = 0;

	int32_t challengeFocusDuration = 0;
	int32_t stepDuration = 0;

	bool ignoreFieldDamage = false;

	int64_t lastMeleeAttack = 0;

	void death(Creature* lastHitCreature) override;
	Item* getCorpse(Creature* lastHitCreature, Creature* mostDamageCreature) override;

	void onAddCondition(ConditionType_t type) override;
	void onEndCondition(ConditionType_t type) override;

	uint64_t getLostExperience() const override { return skillLoss ? mType->info.experience : 0; }
	void dropLoot(Container* corpse, Creature* lastHitCreature) override;
	uint32_t getDamageImmunities() const override { return mType->info.damageImmunities; }
	uint32_t getConditionImmunities() const override { return mType->info.conditionImmunities; }
	void getPathSearchParams(const Creature* creature, FindPathParams& fpp) const override;
	bool useCacheMap() const override { return !randomStepping; }

public:
	Monster() = default;
	explicit Monster(MonsterType* mType) : mType(mType)
	{
		this->setName(mType->name);
		this->setNameDescription(mType->nameDescription);
		this->health = mType->info.health;
		this->healthMax = mType->info.healthMax;
		this->setBaseSpeed(mType->info.baseSpeed);
		this->setCurrentOutfit(mType->info.outfit);
	};
	// non-copyable
	Monster(const Monster&) = delete;
	Monster& operator=(const Monster&) = delete;

	static Monster* createMonsterByName(const std::string& name);
	static uint32_t MONSTER_AUTO_ID;
	static int32_t DESPAWN_RANGE;
	static int32_t DESPAWN_RADIUS;

	static bool pushItem(Item* item);
	static void pushItems(Tile* tile);
	static bool pushCreature(Creature* creature);
	static void pushCreatures(Tile* tile);

	Monster* getMonster() override { return this; }
	const Monster* getMonster() const override { return this; }

	bool isHostile() const { return mType->info.isHostile; }

	void setID() override;
	void addList() override;
	void removeList() override;

	void setName(const std::string& name);
	void setNameDescription(const std::string& nameDescription) { this->nameDescription = nameDescription; }
	const std::string& getName() const override;
	const std::string& getNameDescription() const override;
	std::string getDescription(int32_t) const override;
	CreatureType_t getType() const override { return CREATURETYPE_MONSTER; }
	MonsterType* getMonsterType() const { return mType; }

	const Position& getMasterPos() const { return masterPos; }
	void setMasterPos(Position pos) { masterPos = pos; }
	bool isMasterInRange() const { return masterInRange; }
	void setMasterInRange(bool inRange) { masterInRange = inRange; }

	bool isInSpawnRange(const Position& pos) const;
	bool isWalkingToSpawn() const { return walkingToSpawn; }
	bool walkToSpawn();
	bool canWalkTo(Position pos, Direction direction) const;
	bool canWalkOnFieldType(CombatType_t combatType) const;
	void onWalk() override;
	void onWalkComplete() override;
	bool getNextStep(Direction& direction, uint32_t& flags) override;
	void onFollowCreatureComplete(const Creature* creature) override;
	bool isRandomStepping() const { return randomStepping; }
	void setRandomStepping(bool randomStepping) { this->randomStepping = randomStepping; }
	bool getDistanceStep(const Position& targetPos, Direction& direction, bool flee = false);
	bool getRandomStep(const Position& creaturePos, Direction& direction) const;
	bool getDanceStep(const Position& creaturePos, Direction& direction, bool keepAttack = true,
	                  bool keepDistance = true);

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

	bool canUseSpell(const Position& pos, const Position& targetPos, const MonsterSpell& sb, uint32_t interval,
	                 bool& inRange, bool& resetTicks);
	bool canUseAttack(const Position& pos, const Creature* target) const;
	bool canSee(const Position& pos) const override;
	bool canSeeInvisibility() const override { return isImmune(CONDITION_INVISIBLE); }

	void setSpawn(Spawn* spawn) { this->spawn = spawn; }

	bool isFleeing() const
	{
		return !isSummon() && getHealth() <= mType->info.runAwayHealth && challengeFocusDuration <= 0;
	}

	bool isTargetNearby() const { return stepDuration >= 1; }

	bool canPushItems() const;
	bool canPushCreatures() const { return mType->info.canPushCreatures; }

	bool isIgnoringFieldDamage() const { return ignoreFieldDamage; }

	uint16_t getLookCorpse() const override { return mType->info.lookcorpse; }
	RaceType_t getRace() const { return mType->info.race; }
	int32_t getArmor() const override { return mType->info.armor; }
	int32_t getDefense() const override { return mType->info.defense; }
	bool isPushable() const override { return mType->info.pushable && baseSpeed != 0; }
	bool isAttackable() const override { return mType->info.isAttackable; }

	void onAttackedCreatureDisappear(bool isLogout) override;
	void onCreatureAppear(Creature* creature, bool isLogin) override;
	void onRemoveCreature(Creature* creature, bool isLogout) override;
	void onCreatureMove(Creature* creature, const Tile* newTile, const Position& newPos, const Tile* oldTile,
	                    const Position& oldPos, bool teleport) override;
	void onCreatureSay(Creature* creature, SpeakClasses type, const std::string& text) override;
	void onCreatureEnter(Creature* creature);
	void onCreatureLeave(Creature* creature);
	void onCreatureFound(Creature* creature, bool pushFront = false);

	void drainHealth(Creature* attacker, int32_t damage) override;
	void changeHealth(int32_t healthChange, bool sendHealthChange = true) override;

	void updateTargetList();
	void updateLookDirection();

	void onThink(uint32_t interval) override;
	void onThinkTarget(uint32_t interval);
	void onThinkYell(uint32_t interval);
	void onThinkDefense(uint32_t interval);

	bool challengeCreature(Creature* creature, bool force = false) override;
	void setNormalCreatureLight() override { internalLight = mType->info.light; }
	bool getCombatValues(int32_t& min, int32_t& max) override;
	void doAttacking(uint32_t interval) override;
	bool hasExtraSwing() override { return lastMeleeAttack == 0; }
	BlockType_t blockHit(Creature* attacker, CombatType_t combatType, int32_t& damage, bool checkDefense = false,
	                     bool checkArmor = false, bool field = false, bool ignoreResistances = false) override;
};

#endif
