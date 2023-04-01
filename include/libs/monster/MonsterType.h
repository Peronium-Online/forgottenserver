#ifndef PR_MONSTERTYPE_H
#define PR_MONSTERTYPE_H

#include "constants/const.h"
#include "constants/enums.h"
#include "libs/monster/MonsterSpell.h"
#include "libs/util/xml/XMLElementBuilder.h"

#include <map>
#include <string>
#include <vector>

struct voiceBlock_t
{
	std::string text;
	bool yellText;
};

struct summonBlock_t
{
	std::string name;
	uint32_t chance;
	uint32_t speed;
	uint32_t max;
	bool force = false;
};

struct targetSeekBlock_t
{
	std::string cid;
	uint32_t priority = 1;
};

struct LootBlock
{
	uint16_t id;
	uint32_t countmax;
	uint32_t chance;

	// optional
	int32_t subType;
	int32_t actionId;
	std::string text;

	std::vector<LootBlock> childLoot;
	LootBlock()
	{
		id = 0;
		countmax = 1;
		chance = 0;

		subType = -1;
		actionId = -1;
	}
};

class BaseSpell;

class LuaScriptInterface;

class MonsterType
{
	struct MonsterInfo
	{
		LuaScriptInterface* scriptInterface;

		std::map<CombatType_t, int32_t> elementMap;

		std::vector<voiceBlock_t> voiceVector;

		std::vector<LootBlock> lootItems;
		std::vector<std::string> scripts;
		std::vector<MonsterSpell> attackSpells;
		std::vector<MonsterSpell> defenseSpells;
		std::vector<summonBlock_t> summons;
		std::vector<targetSeekBlock_t> targetSeeks;

		Skulls_t skull = SKULL_NONE;
		Outfit_t outfit = {};
		RaceType_t race = RACE_BLOOD;

		LightInfo light = {};
		uint16_t lookcorpse = 0;

		uint64_t experience = 0;

		uint32_t manaCost = 0;
		uint32_t yellChance = 0;
		uint32_t yellSpeedTicks = 0;
		uint32_t staticAttackChance = 95;
		uint32_t maxSummons = 0;
		uint32_t changeTargetSpeed = 0;
		uint32_t conditionImmunities = 0;
		uint32_t damageImmunities = 0;
		uint32_t baseSpeed = 200;

		int32_t creatureAppearEvent = -1;
		int32_t creatureDisappearEvent = -1;
		int32_t creatureMoveEvent = -1;
		int32_t creatureSayEvent = -1;
		int32_t thinkEvent = -1;
		int32_t targetDistance = 1;
		int32_t runAwayHealth = 0;
		int32_t health = 100;
		int32_t healthMax = 100;
		int32_t changeTargetChance = 0;
		int32_t defense = 0;
		int32_t armor = 0;

		bool canPushItems = false;
		bool canPushCreatures = false;
		bool pushable = true;
		bool isAttackable = true;
		bool isBoss = false;
		bool isChallengeable = true;
		bool isConvinceable = false;
		bool isHostile = true;
		bool isIgnoringSpawnBlock = false;
		bool isIllusionable = false;
		bool isSummonable = false;
		bool hiddenHealth = false;
		bool canWalkOnEnergy = true;
		bool canWalkOnFire = true;
		bool canWalkOnPoison = true;

		MonstersEvent_t eventType = MONSTERS_EVENT_NONE;
	};

public:
	MonsterType() = default;

	// non-copyable
	MonsterType(const MonsterType&) = delete;
	MonsterType& operator=(const MonsterType&) = delete;

	bool loadCallback(LuaScriptInterface* scriptInterface);

	std::string name;
	std::string nameDescription;

	MonsterInfo info;

	void loadLoot(MonsterType* monsterType, LootBlock lootBlock);

	class Builder : public virtual XMLElementBuilder<MonsterType*>
	{
	private:
		MonsterType* mType;

	public:
		virtual MonsterType* loadFromXMLNode(pugi::xml_node node, bool reloading) override;

		virtual MonsterType* build();

		virtual Builder* setName(std::string name);
		virtual Builder* setNameDescription(std::string nameDescription);
		virtual Builder* setRace(std::string raceName);
		virtual Builder* setRace(uint16_t raceNumber);
		virtual Builder* setExperience(uint64_t exp);
		virtual Builder* setSpeed(int32_t speed);
		virtual Builder* setManaCost(uint32_t cost);
		virtual Builder* setSkull(std::string skull);
		virtual Builder* setScript(std::string filename);
		virtual Builder* setHealthNow(int32_t healthNow);
		virtual Builder* setHealthMax(int32_t healthMax);
		virtual Builder* setSummonable(bool summonable);
		virtual Builder* setAttackable(bool attackable);
		virtual Builder* setHostile(bool hostile);
		virtual Builder* setIgnoreSpawnBlock(bool ignore);
		virtual Builder* setIllusionable(bool illusionble);
		virtual Builder* setChallengeable(bool challengeable);
		virtual Builder* setConvinceable(bool convinceable);
		virtual Builder* setPushable(bool pushable);
		virtual Builder* setBoss(bool boss);
		virtual Builder* setCanPushItems(bool canPushItems);
		virtual Builder* setCanPushCreatures(bool canPushCreatures);
		virtual Builder* setStaticAttack(uint32_t staticAttack);
		virtual Builder* setLightLevel(uint16_t lightLevel);
		virtual Builder* setLightColor(uint16_t lightColor);
		virtual Builder* setTargetDistance(int32_t targetDistance);
		virtual Builder* setRunOnHealth(int32_t runOnHealth);
		virtual Builder* setHideHealth(bool hideHealth);
		virtual Builder* setCanWalkOnEnergy(bool canWalk);
		virtual Builder* setCanWalkOnFire(bool canWalk);
		virtual Builder* setCanWalkOnPoison(bool canWalk);
		virtual Builder* setTargetChange(uint32_t interval, int32_t chance);
		virtual Builder* setLook(Outfit_t outfit, uint16_t corpse);
	};
};

#endif
