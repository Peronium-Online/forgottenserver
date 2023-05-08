#ifndef PR_MONSTERTYPE_H
#define PR_MONSTERTYPE_H

#include "constants/const.h"
#include "constants/enums.h"
#include "libs/monster/MonsterLoot.h"
#include "libs/monster/MonsterSeek.h"
#include "libs/monster/MonsterSpell.h"
#include "libs/monster/MonsterSummon.h"
#include "libs/monster/MonsterVoice.h"
#include "libs/monster/Monsters.h"
#include "libs/util/tools/gamehelpers.h"
#include "libs/util/xml/XMLElementBuilder.h"

#include <boost/algorithm/string.hpp>
#include <map>
#include <string>
#include <vector>

extern Monsters g_monsters;

class MonsterType
{
	struct MonsterInfo
	{
		LuaScriptInterface* scriptInterface;

		std::map<CombatType_t, int32_t> elementMap;

		MonsterVoice voice;

		std::vector<MonsterLoot> lootItems;
		std::vector<std::string> scripts;
		std::vector<MonsterSpell> attackSpells;
		std::vector<MonsterSpell> defenseSpells;
		std::vector<MonsterSummon> summons;
		std::vector<MonsterSeek> seeks;

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

	class Builder : virtual public XMLLoadable
	{
	private:
		MonsterType* mType;

		virtual bool load(pugi::xml_node node, bool reloading) override;

	public:
		Builder() = default;
		Builder(std::string filepath)
		{
			this->filepath = filepath;
			this->childNode = "monster";
		};

		virtual MonsterType* build();

		Builder* setName(std::string name)
		{
			std::string lowerCasedMonsterName = boost::algorithm::to_lower_copy(name);

			this->mType->name = name;
			this->mType->nameDescription = "a " + lowerCasedMonsterName;
			return this;
		}

		Builder* setNameDescription(std::string nameDescription)
		{
			this->mType->nameDescription = nameDescription;
			return this;
		}

		Builder* setRace(std::string raceName)
		{
			if (raceName == "venom") {
				this->mType->info.race = RACE_VENOM;
			} else if (raceName == "blood") {
				this->mType->info.race = RACE_BLOOD;
			} else if (raceName == "undead") {
				this->mType->info.race = RACE_UNDEAD;
			} else if (raceName == "fire") {
				this->mType->info.race = RACE_FIRE;
			} else if (raceName == "energy") {
				this->mType->info.race = RACE_ENERGY;
			} else if (raceName == "ink") {
				this->mType->info.race = RACE_INK;
			}

			return this;
		}

		Builder* setRace(uint16_t raceNumber)
		{
			switch (raceNumber) {
				case 1:
					this->mType->info.race = RACE_VENOM;
					break;
				case 2:
					this->mType->info.race = RACE_BLOOD;
					break;
				case 3:
					this->mType->info.race = RACE_UNDEAD;
					break;
				case 4:
					this->mType->info.race = RACE_FIRE;
					break;
				case 5:
					this->mType->info.race = RACE_ENERGY;
					break;
				case 6:
					this->mType->info.race = RACE_INK;
					break;
				default:
					break;
			}

			return this;
		}

		Builder* setExperience(uint64_t exp)
		{
			this->mType->info.experience = exp;
			return this;
		}

		Builder* setSpeed(int32_t speed)
		{
			this->mType->info.baseSpeed = speed;
			return this;
		}

		Builder* setSkull(std::string skull)
		{
			this->mType->info.skull = getSkullType(boost::algorithm::to_lower_copy<std::string>(skull));
			return this;
		}

		Builder* setScript(std::string filename)
		{
			if (!g_monsters.scriptInterface) {
				g_monsters.scriptInterface.reset(new LuaScriptInterface("Monster Interface"));
				g_monsters.scriptInterface->initState();
			}

			if (g_monsters.scriptInterface->loadFile("data/monster/scripts/" + filename) == 0) {
				mType->info.scriptInterface = g_monsters.scriptInterface.get();
				mType->info.creatureAppearEvent = g_monsters.scriptInterface->getEvent("onCreatureAppear");
				mType->info.creatureDisappearEvent = g_monsters.scriptInterface->getEvent("onCreatureDisappear");
				mType->info.creatureMoveEvent = g_monsters.scriptInterface->getEvent("onCreatureMove");
				mType->info.creatureSayEvent = g_monsters.scriptInterface->getEvent("onCreatureSay");
				mType->info.thinkEvent = g_monsters.scriptInterface->getEvent("onThink");
			} else {
				std::cout << "[Warning - MonsterType::setScript] Can not load script: " << filename << std::endl;
				std::cout << g_monsters.scriptInterface->getLastLuaError() << std::endl;
			}

			return this;
		}

		Builder* setHealthNow(int32_t healthNow)
		{
			this->mType->info.health = healthNow;
			return this;
		}

		Builder* setHealthMax(int32_t healthMax)
		{
			this->mType->info.healthMax = healthMax;
			return this;
		}

		Builder* setManaCost(uint32_t cost)
		{
			this->mType->info.manaCost = cost;
			return this;
		}

		Builder* setSummonable(bool summonable)
		{
			this->mType->info.isSummonable = summonable;
			return this;
		}

		Builder* setAttackable(bool attackable)
		{
			this->mType->info.isAttackable = attackable;
			return this;
		}

		Builder* setHostile(bool hostile)
		{
			this->mType->info.isHostile = hostile;
			return this;
		}

		Builder* setIgnoreSpawnBlock(bool ignore)
		{
			this->mType->info.isIgnoringSpawnBlock = ignore;
			return this;
		}

		Builder* setIllusionable(bool illusionable)
		{
			this->mType->info.isIllusionable = illusionable;
			return this;
		}

		Builder* setChallengeable(bool challengeable)
		{
			this->mType->info.isChallengeable = challengeable;
			return this;
		}

		Builder* setConvinceable(bool convinceable)
		{
			this->mType->info.isConvinceable = convinceable;
			return this;
		}

		Builder* setPushable(bool pushable)
		{
			if (pushable) {
				this->mType->info.canPushCreatures = false;
			}
			this->mType->info.pushable = pushable;

			return this;
		}

		Builder* setBoss(bool boss)
		{
			this->mType->info.isBoss = boss;
			return this;
		}

		Builder* setCanPushItems(bool canPushItems)
		{
			this->mType->info.canPushItems = canPushItems;
			return this;
		}

		Builder* setCanPushCreatures(bool canPush)
		{
			if (canPush) {
				this->mType->info.pushable = false;
			}
			this->mType->info.canPushCreatures = canPush;

			return this;
		}

		Builder* setStaticAttack(uint32_t staticAttack)
		{
			if (staticAttack > 100) {
				std::cout << "[Warning - MonsterType::loadFromXMLNode] staticattack greater than 100. "
				          << this->mType->name << std::endl;
				staticAttack = 100;
			}

			this->mType->info.staticAttackChance = staticAttack;

			return this;
		}

		Builder* setLightLevel(uint16_t lightLevel)
		{
			this->mType->info.light.level = lightLevel;
			return this;
		}

		Builder* setLightColor(uint16_t lightColor)
		{
			this->mType->info.light.color = lightColor;
			return this;
		}

		Builder* setTargetDistance(int32_t targetDistance)
		{
			if (targetDistance < 1) {
				targetDistance = 1;
				std::cout << "[Warning - MonsterType::loadFromXMLNode] targetdistance less than 1. "
				          << this->mType->name << std::endl;
			}

			this->mType->info.targetDistance = targetDistance;
			return this;
		}

		Builder* setRunOnHealth(int32_t runAwayHealth)
		{
			this->mType->info.runAwayHealth = runAwayHealth;
			return this;
		}

		Builder* setHideHealth(bool hideHealth)
		{
			this->mType->info.hiddenHealth = hideHealth;
			return this;
		}

		Builder* setCanWalkOnEnergy(bool canWalk)
		{
			this->mType->info.canWalkOnEnergy = canWalk;
			return this;
		}

		Builder* setCanWalkOnFire(bool canWalk)
		{
			this->mType->info.canWalkOnFire = canWalk;
			return this;
		}

		Builder* setCanWalkOnPoison(bool canWalk)
		{
			this->mType->info.canWalkOnPoison = canWalk;
			return this;
		}

		Builder* setTargetChange(uint32_t interval, int32_t chance)
		{
			if (chance > 100) {
				chance = 100;
				std::cout << "[Warning - MonsterType::loadFromXMLNode] targetchange chance value out of bounds. "
				          << this->mType->name << std::endl;
			}

			this->mType->info.changeTargetSpeed = interval;
			this->mType->info.changeTargetChance = chance;

			return this;
		}

		Builder* setLook(Outfit_t outfit, uint16_t corpse)
		{
			this->mType->info.outfit = outfit;
			this->mType->info.lookcorpse = corpse;

			return this;
		}

		Builder* addAttackSpell(MonsterSpell spell)
		{
			this->mType->info.attackSpells.emplace_back(std::move(spell));
			return this;
		}

		Builder* setDefense(int32_t defense)
		{
			this->mType->info.defense = defense;
			return this;
		}

		Builder* setArmor(int32_t armor)
		{
			this->mType->info.armor = armor;
			return this;
		}

		Builder* addDefenseSpell(MonsterSpell spell)
		{
			this->mType->info.defenseSpells.emplace_back(std::move(spell));
			return this;
		}

		Builder* withImmunityToPhysical()
		{
			this->mType->info.damageImmunities |= COMBAT_PHYSICALDAMAGE;
			this->mType->info.conditionImmunities |= CONDITION_BLEEDING;
			return this;
		}

		Builder* withImmunityToPhysical(int32_t percentage)
		{
			this->mType->info.elementMap[COMBAT_PHYSICALDAMAGE] = percentage;
			if (this->mType->info.damageImmunities & COMBAT_PHYSICALDAMAGE) {
				std::cout
				    << "[Warning - MonsterType::withImmunityToPhysical] Same element \"physical\" on immunity and element tags. "
				    << this->mType->name << std::endl;
			}

			return this;
		}

		Builder* withImmunityToFire()
		{
			this->mType->info.damageImmunities |= COMBAT_FIREDAMAGE;
			this->mType->info.conditionImmunities |= CONDITION_FIRE;
			return this;
		}

		Builder* withImmunityToFire(int32_t percentage)
		{
			this->mType->info.elementMap[COMBAT_FIREDAMAGE] = percentage;
			if (this->mType->info.damageImmunities & COMBAT_FIREDAMAGE) {
				std::cout
				    << "[Warning - MonsterType::withImmunityToFire] Same element \"fire\" on immunity and element tags. "
				    << this->mType->name << std::endl;
			}

			return this;
		}

		Builder* withImmunityToPoison()
		{
			this->mType->info.damageImmunities |= COMBAT_EARTHDAMAGE;
			this->mType->info.conditionImmunities |= CONDITION_POISON;
			return this;
		}

		Builder* withImmunityToPoison(int32_t percentage)
		{
			this->mType->info.elementMap[COMBAT_EARTHDAMAGE] = percentage;
			if (this->mType->info.damageImmunities & COMBAT_EARTHDAMAGE) {
				std::cout
				    << "[Warning - MonsterType::withImmunityToPoison] Same element \"earth\" on immunity and element tags. "
				    << this->mType->name << std::endl;
			}

			return this;
		}

		Builder* withImmunityToEnergy()
		{
			this->mType->info.damageImmunities |= COMBAT_ENERGYDAMAGE;
			this->mType->info.conditionImmunities |= CONDITION_ENERGY;
			return this;
		}

		Builder* withImmunityToEnergy(int32_t percentage)
		{
			this->mType->info.elementMap[COMBAT_ENERGYDAMAGE] = percentage;
			if (this->mType->info.damageImmunities & COMBAT_ENERGYDAMAGE) {
				std::cout
				    << "[Warning - MonsterType::withImmunityToEnergy] Same element \"energy\" on immunity and element tags. "
				    << this->mType->name << std::endl;
			}

			return this;
		}

		Builder* withImmunityToDrown()
		{
			this->mType->info.damageImmunities |= COMBAT_DROWNDAMAGE;
			this->mType->info.conditionImmunities |= CONDITION_DROWN;
			return this;
		}

		Builder* withImmunityToDrown(int32_t percentage)
		{
			this->mType->info.elementMap[COMBAT_DROWNDAMAGE] = percentage;
			if (this->mType->info.damageImmunities & COMBAT_DROWNDAMAGE) {
				std::cout
				    << "[Warning - MonsterType::withImmunityToDrown] Same element \"drown\" on immunity and element tags. "
				    << this->mType->name << std::endl;
			}

			return this;
		}

		Builder* withImmunityToIce()
		{
			this->mType->info.damageImmunities |= COMBAT_ICEDAMAGE;
			this->mType->info.conditionImmunities |= CONDITION_FREEZING;
			return this;
		}

		Builder* withImmunityToIce(int32_t percentage)
		{
			this->mType->info.elementMap[COMBAT_ICEDAMAGE] = percentage;
			if (this->mType->info.damageImmunities & COMBAT_ICEDAMAGE) {
				std::cout
				    << "[Warning - MonsterType::withImmunityToIce] Same element \"ice\" on immunity and element tags. "
				    << this->mType->name << std::endl;
			}

			return this;
		}

		Builder* withImmunityToHoly()
		{
			this->mType->info.damageImmunities |= COMBAT_HOLYDAMAGE;
			this->mType->info.conditionImmunities |= CONDITION_DAZZLED;
			return this;
		}

		Builder* withImmunityToHoly(int32_t percentage)
		{
			this->mType->info.elementMap[COMBAT_HOLYDAMAGE] = percentage;
			if (this->mType->info.damageImmunities & COMBAT_HOLYDAMAGE) {
				std::cout
				    << "[Warning - MonsterType::withImmunityToHoly] Same element \"holy\" on immunity and element tags. "
				    << this->mType->name << std::endl;
			}

			return this;
		}

		Builder* withImmunityToDeath()
		{
			this->mType->info.damageImmunities |= COMBAT_DEATHDAMAGE;
			this->mType->info.conditionImmunities |= CONDITION_CURSED;
			return this;
		}

		Builder* withImmunityToDeath(int32_t percentage)
		{
			this->mType->info.elementMap[COMBAT_DEATHDAMAGE] = percentage;
			if (this->mType->info.damageImmunities & COMBAT_DEATHDAMAGE) {
				std::cout
				    << "[Warning - MonsterType::withImmunityToDeath] Same element \"death\" on immunity and element tags. "
				    << this->mType->name << std::endl;
			}

			return this;
		}

		Builder* withImmunityToLifeDrain()
		{
			this->mType->info.damageImmunities |= COMBAT_LIFEDRAIN;
			return this;
		}

		Builder* withImmunityToLifeDrain(int32_t percentage)
		{
			this->mType->info.elementMap[COMBAT_LIFEDRAIN] = percentage;
			if (this->mType->info.damageImmunities & COMBAT_LIFEDRAIN) {
				std::cout
				    << "[Warning - MonsterType::withImmunityToLifeDrain] Same element \"lifedrain\" on immunity and element tags. "
				    << this->mType->name << std::endl;
			}

			return this;
		}

		Builder* withImmunityToManaDrain()
		{
			this->mType->info.damageImmunities |= COMBAT_MANADRAIN;
			return this;
		}

		Builder* withImmunityToManaDrain(int32_t percentage)
		{
			this->mType->info.elementMap[COMBAT_MANADRAIN] = percentage;
			if (this->mType->info.damageImmunities & COMBAT_MANADRAIN) {
				std::cout
				    << "[Warning - MonsterType::withImmunityToManaDrain] Same element \"manadrain\" on immunity and element tags. "
				    << this->mType->name << std::endl;
			}

			return this;
		}

		Builder* withImmunityToParalyze()
		{
			this->mType->info.conditionImmunities |= CONDITION_PARALYZE;
			return this;
		}

		Builder* withImmunityToOutfitChange()
		{
			this->mType->info.conditionImmunities |= CONDITION_OUTFIT;
			return this;
		}

		Builder* withImmunityToDrunk()
		{
			this->mType->info.conditionImmunities |= CONDITION_DRUNK;
			return this;
		}

		Builder* withImmunityToInvisibility()
		{
			this->mType->info.conditionImmunities |= CONDITION_INVISIBLE;
			return this;
		}

		Builder* withImmunityToBleed()
		{
			this->mType->info.conditionImmunities |= CONDITION_BLEEDING;
			return this;
		}

		Builder* setVoice(MonsterVoice voice)
		{
			this->mType->info.voice = voice;
			return this;
		}

		Builder* addLoot(MonsterLoot loot)
		{
			this->mType->info.lootItems.emplace_back(std::move(loot));
			return this;
		}

		Builder* setMaxSummons(uint32_t maxSummons)
		{
			this->mType->info.maxSummons = std::min(maxSummons, MAX_SUMMONS);
			return this;
		}

		Builder* addSummon(MonsterSummon summon)
		{
			this->mType->info.summons.emplace_back(std::move(summon));
			return this;
		}

		Builder* addSeek(MonsterSeek seek)
		{
			this->mType->info.seeks.emplace_back(std::move(seek));
			return this;
		}

		Builder* addEvent(std::string script)
		{
			this->mType->info.scripts.emplace_back(std::move(script));
			return this;
		}
	};
};

#endif
