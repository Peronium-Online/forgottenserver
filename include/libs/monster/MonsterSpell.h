#ifndef PR_MONSTER_SPELL_H
#define PR_MONSTER_SPELL_H

#include "../../../src/combat.h"
#include "../../../src/spells.h"

class LMonsterSpell
{
public:
	LMonsterSpell() = default;

	LMonsterSpell(const LMonsterSpell&) = delete;
	LMonsterSpell& operator=(const LMonsterSpell&) = delete;

	std::string name = "";
	std::string scriptName = "";

	uint8_t chance = 100;
	uint8_t range = 0;
	uint8_t drunkenness = 0;

	uint16_t interval = 2000;

	int32_t minCombatValue = 0;
	int32_t maxCombatValue = 0;
	int32_t attack = 0;
	int32_t skill = 0;
	int32_t length = 0;
	int32_t spread = 0;
	int32_t radius = 0;
	int32_t ring = 0;
	int32_t conditionMinDamage = 0;
	int32_t conditionMaxDamage = 0;
	int32_t conditionStartDamage = 0;
	int32_t tickInterval = 0;
	int32_t minSpeedChange = 0;
	int32_t maxSpeedChange = 0;
	int32_t duration = 0;

	bool isScripted = false;
	bool needTarget = false;
	bool needDirection = false;
	bool combatSpell = false;
	bool isMelee = false;

	Outfit_t outfit = {};
	ShootType_t shoot = CONST_ANI_NONE;
	MagicEffectClasses effect = CONST_ME_NONE;
	ConditionType_t conditionType = CONDITION_NONE;
	CombatType_t combatType = COMBAT_UNDEFINEDDAMAGE;
};

class MonsterSpell
{
public:
	MonsterSpell() = default;
	MonsterSpell(const std::string& name, const std::string& scriptName);
	MonsterSpell(const MonsterSpell& other) = delete;
	MonsterSpell& operator=(const MonsterSpell& other) = delete;
	MonsterSpell(MonsterSpell&& other) :
	    spell(other.spell),
	    chance(other.chance),
	    speed(other.speed),
	    range(other.range),
	    minCombatValue(other.minCombatValue),
	    maxCombatValue(other.maxCombatValue),
	    combatSpell(other.combatSpell),
	    isMelee(other.isMelee),
	    name(other.name),
	    scriptName(other.scriptName)
	{
		other.spell = nullptr;
	}

	std::string name = "";
	std::string scriptName = "";

	BaseSpell* spell = nullptr;
	uint32_t chance = 100;
	uint32_t speed = 2000;
	uint32_t range = 0;
	int32_t minCombatValue = 0;
	int32_t maxCombatValue = 0;

	bool combatSpell = false;
	bool isMelee = false;
	bool isScripted = false;

	virtual std::string getName();

	virtual MonsterSpell* setSpeed(int32_t speed);
	virtual MonsterSpell* setChance(uint32_t chance);
	virtual MonsterSpell* setRange(uint32_t range);
	virtual MonsterSpell* setMinCombatValue(int32_t value);
	virtual MonsterSpell* setMaxCombatValue(int32_t value);
	virtual MonsterSpell* setBaseSpell(BaseSpell* spell);
	virtual MonsterSpell* setSpellFromScript(bool needTarget, bool needDirection);
	virtual MonsterSpell* setMeleeAttack(int32_t attack, int32_t skill);

	class Factory : virtual public XMLElementBuilder<MonsterSpell*>
	{
	public:
		static std::shared_ptr<MonsterSpell> deserializeSpellFromLua(LMonsterSpell* spell);

		virtual MonsterSpell* loadFromXMLNode(pugi::xml_node node, bool reloading) override;
	};

	class CombatBuilder
	{
	private:
		bool needTarget = false;
		bool needDirection = false;
		Combat_ptr combat = std::make_shared<Combat>();

		ConditionDamage* getDamageCondition(ConditionType_t conditionType, int32_t maxDamage, int32_t minDamage,
		                                    int32_t startDamage, uint32_t tickInterval)
		{
			ConditionDamage* condition =
			    static_cast<ConditionDamage*>(Condition::createCondition(CONDITIONID_COMBAT, conditionType, 0, 0));
			condition->setParam(CONDITION_PARAM_TICKINTERVAL, tickInterval);
			condition->setParam(CONDITION_PARAM_MINVALUE, minDamage);
			condition->setParam(CONDITION_PARAM_MAXVALUE, maxDamage);
			condition->setParam(CONDITION_PARAM_STARTVALUE, startDamage);
			condition->setParam(CONDITION_PARAM_DELAYED, 1);
			return condition;
		}

	public:
		Combat_ptr build() { return combat; }

		bool isTargetNeeded() { return needTarget; }

		bool isDirectionNeeded() { return needDirection; }

		CombatBuilder* withLength(int32_t length, int32_t spread = 3)
		{
			if (length <= 0) {
				return this;
			}

			AreaCombat* area = new AreaCombat();
			area->setupArea(length, std::max<int32_t>(0, spread));
			combat->setArea(area);

			needDirection = true;

			return this;
		}

		CombatBuilder* withRadius(int32_t radius, bool needTarget = false)
		{
			if (radius <= 0) {
				return this;
			}

			AreaCombat* area = new AreaCombat();
			area->setupArea(radius);
			combat->setArea(area);

			this->needTarget = needTarget;

			return this;
		}

		CombatBuilder* withRing(int32_t ring, bool needTarget = false)
		{
			if (ring <= 0) {
				return this;
			}

			AreaCombat* area = new AreaCombat();
			area->setupAreaRing(ring);
			combat->setArea(area);

			this->needTarget = needTarget;

			return this;
		}

		CombatBuilder* withMelee()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_PHYSICALDAMAGE);
			combat->setParam(COMBAT_PARAM_BLOCKARMOR, 1);
			combat->setParam(COMBAT_PARAM_BLOCKSHIELD, 1);
			combat->setOrigin(ORIGIN_MELEE);

			return this;
		}

		CombatBuilder* withCondition(ConditionType_t type, int32_t minDamage, int32_t maxDamage,
		                             uint32_t tickInterval = 2000, int32_t startDamage = 0)
		{
			if (type == CONDITION_NONE) {
				return this;
			}

			if (tickInterval <= 0) {
				tickInterval = 2000;
			}

			Condition* condition = getDamageCondition(type, maxDamage, minDamage, startDamage, tickInterval);
			combat->addCondition(condition);
			return this;
		}

		CombatBuilder* withConditionFire(int32_t damage, uint32_t tickInterval = 9000)
		{
			return this->withConditionFire(damage, damage, tickInterval);
		}

		CombatBuilder* withConditionFire(int32_t minDamage, int32_t maxDamage, uint32_t tickInterval = 10000,
		                                 int32_t startDamage = 0)
		{
			if (tickInterval <= 0) {
				tickInterval = 10000;
			}

			return this->withCondition(CONDITION_FIRE, minDamage, maxDamage, tickInterval, startDamage);
		}

		CombatBuilder* withConditionPoison(int32_t damage, uint32_t tickInterval = 4000)
		{
			return this->withConditionPoison(damage, damage, tickInterval);
		}

		CombatBuilder* withConditionPoison(int32_t minDamage, int32_t maxDamage, uint32_t tickInterval = 4000,
		                                   int32_t startDamage = 0)
		{
			if (tickInterval <= 0) {
				tickInterval = 4000;
			}

			return this->withCondition(CONDITION_POISON, minDamage, maxDamage, tickInterval, startDamage);
		}

		CombatBuilder* withConditionEnergy(int32_t damage, uint32_t tickInterval = 10000)
		{
			return this->withConditionEnergy(damage, damage, tickInterval);
		}

		CombatBuilder* withConditionEnergy(int32_t minDamage, int32_t maxDamage, uint32_t tickInterval = 10000,
		                                   int32_t startDamage = 0)
		{
			if (tickInterval <= 0) {
				tickInterval = 10000;
			}

			return this->withCondition(CONDITION_ENERGY, minDamage, maxDamage, tickInterval, startDamage);
		}

		CombatBuilder* withConditionDrown(int32_t damage, uint32_t tickInterval = 5000)
		{
			return this->withConditionDrown(damage, damage, tickInterval);
		}

		CombatBuilder* withConditionDrown(int32_t minDamage, int32_t maxDamage, uint32_t tickInterval = 5000,
		                                  int32_t startDamage = 0)
		{
			if (tickInterval <= 0) {
				tickInterval = 5000;
			}

			return this->withCondition(CONDITION_DROWN, minDamage, maxDamage, tickInterval, startDamage);
		}

		CombatBuilder* withConditionFreeze(int32_t damage, uint32_t tickInterval = 8000)
		{
			return this->withConditionFreeze(damage, damage, tickInterval);
		}

		CombatBuilder* withConditionFreeze(int32_t minDamage, int32_t maxDamage, uint32_t tickInterval = 8000,
		                                   int32_t startDamage = 0)
		{
			if (tickInterval <= 0) {
				tickInterval = 8000;
			}

			return this->withCondition(CONDITION_FREEZING, minDamage, maxDamage, tickInterval, startDamage);
		}

		CombatBuilder* withConditionDazzle(int32_t damage, uint32_t tickInterval = 10000)
		{
			return this->withConditionDazzle(damage, damage, tickInterval);
		}

		CombatBuilder* withConditionDazzle(int32_t minDamage, int32_t maxDamage, uint32_t tickInterval = 10000,
		                                   int32_t startDamage = 0)
		{
			if (tickInterval <= 0) {
				tickInterval = 10000;
			}

			return this->withCondition(CONDITION_DAZZLED, minDamage, maxDamage, tickInterval, startDamage);
		}

		CombatBuilder* withConditionCurse(int32_t damage, uint32_t tickInterval = 4000)
		{
			return this->withConditionCurse(damage, damage, tickInterval);
		}

		CombatBuilder* withConditionCurse(int32_t minDamage, int32_t maxDamage, uint32_t tickInterval = 4000,
		                                  int32_t startDamage = 0)
		{
			if (tickInterval <= 0) {
				tickInterval = 4000;
			}

			return this->withCondition(CONDITION_CURSED, minDamage, maxDamage, tickInterval, startDamage);
		}

		CombatBuilder* withConditionBleed(int32_t damage, uint32_t tickInterval = 4000)
		{
			return this->withConditionBleed(damage, damage, tickInterval);
		}

		CombatBuilder* withConditionBleed(int32_t minDamage, int32_t maxDamage, uint32_t tickInterval = 4000,
		                                  int32_t startDamage = 0)
		{
			if (tickInterval <= 0) {
				tickInterval = 4000;
			}

			return this->withCondition(CONDITION_BLEEDING, minDamage, maxDamage, tickInterval, startDamage);
		}

		CombatBuilder* withCombatType(CombatType_t type)
		{
			combat->setParam(COMBAT_PARAM_TYPE, type);
			return this;
		}

		CombatBuilder* withPhysicalDamage()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_PHYSICALDAMAGE);
			combat->setParam(COMBAT_PARAM_BLOCKARMOR, 1);
			combat->setOrigin(ORIGIN_RANGED);
			return this;
		}

		CombatBuilder* withBleedDamage()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_PHYSICALDAMAGE);

			return this;
		}

		CombatBuilder* withEarthDamage()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_EARTHDAMAGE);

			return this;
		}

		CombatBuilder* withFireDamage()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_FIREDAMAGE);

			return this;
		}

		CombatBuilder* withEnergyDamage()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_ENERGYDAMAGE);

			return this;
		}

		CombatBuilder* withDrownDamage()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_DROWNDAMAGE);

			return this;
		}

		CombatBuilder* withIceDamage()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_ICEDAMAGE);

			return this;
		}

		CombatBuilder* withHolyDamage()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_HOLYDAMAGE);

			return this;
		}

		CombatBuilder* withDeathDamage()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_DEATHDAMAGE);

			return this;
		}

		CombatBuilder* withLifeDrainDamage()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_LIFEDRAIN);

			return this;
		}

		CombatBuilder* withManaDrainDamage()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_MANADRAIN);

			return this;
		}

		CombatBuilder* withHealing()
		{
			combat->setParam(COMBAT_PARAM_TYPE, COMBAT_HEALING);
			combat->setParam(COMBAT_PARAM_AGGRESSIVE, 0);

			return this;
		}

		CombatBuilder* withSpeedChange(int32_t min = 0, int32_t max = 0, int32_t duration = 10000)
		{
			if (min < -1000) {
				std::cout
				    << "[Warning - MonsterSpell::CombatBuilder::withSpeedChange] You cannot reduce a creatures speed below -1000 (100%)"
				    << std::endl;
				min = -1000;
			}

			if (max == 0 || max < min) {
				max = min; // static speed change value
			}

			ConditionType_t conditionType;
			if (min >= 0) {
				conditionType = CONDITION_HASTE;
				combat->setParam(COMBAT_PARAM_AGGRESSIVE, 0);
			} else {
				conditionType = CONDITION_PARALYZE;
			}

			ConditionSpeed* condition = static_cast<ConditionSpeed*>(
			    Condition::createCondition(CONDITIONID_COMBAT, conditionType, duration, 0));
			condition->setFormulaVars(min / 1000.0, 0, max / 1000.0, 0);
			combat->addCondition(condition);

			return this;
		}

		CombatBuilder* withOutfitChange(Outfit_t outfit, int32_t duration = 10000)
		{
			if (duration <= 0) {
				duration = 10000;
			}

			ConditionOutfit* condition = static_cast<ConditionOutfit*>(
			    Condition::createCondition(CONDITIONID_COMBAT, CONDITION_OUTFIT, duration, 0));
			condition->setOutfit(outfit);
			combat->setParam(COMBAT_PARAM_AGGRESSIVE, 0);
			combat->addCondition(condition);

			return this;
		}

		CombatBuilder* withOutfitChange(uint16_t lookTypeEx, int32_t duration = 10000)
		{
			Outfit_t outfit;
			outfit.lookTypeEx = lookTypeEx;

			return this->withOutfitChange(outfit, duration);
		}

		CombatBuilder* withInvisibility(int32_t duration = 10000)
		{
			if (duration <= 0) {
				duration = 10000;
			}

			Condition* condition = Condition::createCondition(CONDITIONID_COMBAT, CONDITION_INVISIBLE, duration, 0);
			combat->setParam(COMBAT_PARAM_AGGRESSIVE, 0);
			combat->addCondition(condition);

			return this;
		}

		CombatBuilder* withDrunk(int32_t duration = 10000, uint8_t level = 25)
		{
			if (duration <= 0) {
				duration = 10000;
			}

			if (level <= 0) {
				level = 25;
			}

			Condition* condition = Condition::createCondition(CONDITIONID_COMBAT, CONDITION_DRUNK, duration, level);
			combat->addCondition(condition);

			return this;
		}

		CombatBuilder* withFireField()
		{
			combat->setParam(COMBAT_PARAM_CREATEITEM, ITEM_FIREFIELD_PVP_FULL);
			return this;
		}

		CombatBuilder* withPoisonField()
		{
			combat->setParam(COMBAT_PARAM_CREATEITEM, ITEM_POISONFIELD_PVP);
			return this;
		}

		CombatBuilder* withEnergyField()
		{
			combat->setParam(COMBAT_PARAM_CREATEITEM, ITEM_ENERGYFIELD_PVP);
			return this;
		}

		CombatBuilder* withShootType(ShootType_t shoot)
		{
			if (shoot != CONST_ANI_NONE) {
				combat->setParam(COMBAT_PARAM_DISTANCEEFFECT, shoot);
			}
			return this;
		}

		CombatBuilder* withMagicEffect(MagicEffectClasses effect)
		{
			if (effect != CONST_ME_NONE) {
				combat->setParam(COMBAT_PARAM_EFFECT, effect);
			}
			return this;
		}
	};
};

// Lua reference data struct

#endif
