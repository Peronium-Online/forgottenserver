#ifndef PR_MONSTER_SPELL
#define PR_MONSTER_SPELL

#include "../../../src/combat.h"
#include "../../../src/spells.h"

class MonsterSpell : virtual public XMLElementBuilder<MonsterSpell*>
{
public:
	MonsterSpell() = default;
	~MonsterSpell();
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
	    isMelee(other.isMelee)
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

	virtual MonsterSpell* loadFromXMLNode(pugi::xml_node node, bool reloading) override;

	virtual bool deserializeSpellFromLua(LMonsterSpell* spell, MonsterSpell& sb, const std::string& description);

	virtual std::string getName();

	virtual MonsterSpell* setSpeed(int32_t speed);
	virtual MonsterSpell* setChance(uint32_t chance);
	virtual MonsterSpell* setRange(uint32_t range);
	virtual MonsterSpell* setMinCombatValue(int32_t value);
	virtual MonsterSpell* setMaxCombatValue(int32_t value);
	virtual MonsterSpell* setBaseSpell(BaseSpell* spell);
	virtual MonsterSpell* setSpellFromScript(bool needTarget, bool needDirection);
	virtual MonsterSpell* setMeleeAttack(int32_t attack, int32_t skill);

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
		Combat_ptr getInstance() { return combat; }

		CombatBuilder* withLength(int32_t length, int32_t spread = 3)
		{
			if (length <= 0) {
				return nullptr;
			}

			AreaCombat* area = new AreaCombat();
			area->setupArea(length, spread);
			combat->setArea(area);

			needDirection = true;

			return this;
		}

		CombatBuilder* withRadius(int32_t radius, bool needTarget = false)
		{
			AreaCombat* area = new AreaCombat();
			area->setupArea(radius);
			combat->setArea(area);

			this->needTarget = needTarget;

			return this;
		}

		CombatBuilder* withRing(int32_t ring, bool needTarget = false)
		{
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

		CombatBuilder* withConditionFire(int32_t damage, uint32_t tickInterval = 9000)
		{
			if (tickInterval <= 0) {
				tickInterval = 9000;
			}

			Condition* condition = getDamageCondition(CONDITION_FIRE, damage, damage, 0, tickInterval);
			combat->addCondition(condition);

			return this;
		}

		CombatBuilder* withConditionPoison(int32_t damage, uint32_t tickInterval = 4000)
		{
			if (tickInterval <= 0) {
				tickInterval = 4000;
			}

			Condition* condition = getDamageCondition(CONDITION_POISON, damage, damage, 0, tickInterval);
			combat->addCondition(condition);

			return this;
		}

		CombatBuilder* withConditionEnergy(int32_t damage, uint32_t tickInterval = 10000)
		{
			if (tickInterval <= 0) {
				tickInterval = 10000;
			}

			Condition* condition = getDamageCondition(CONDITION_ENERGY, damage, damage, 0, tickInterval);
			combat->addCondition(condition);

			return this;
		}

		CombatBuilder* withConditionDrown(int32_t damage, uint32_t tickInterval = 5000)
		{
			if (tickInterval <= 0) {
				tickInterval = 5000;
			}

			Condition* condition = getDamageCondition(CONDITION_DROWN, damage, damage, 0, tickInterval);
			combat->addCondition(condition);

			return this;
		}

		CombatBuilder* withConditionFreeze(int32_t damage, uint32_t tickInterval = 8000)
		{
			if (tickInterval <= 0) {
				tickInterval = 8000;
			}

			Condition* condition = getDamageCondition(CONDITION_FREEZING, damage, damage, 0, tickInterval);
			combat->addCondition(condition);

			return this;
		}

		CombatBuilder* withConditionDazzle(int32_t damage, uint32_t tickInterval = 10000)
		{
			if (tickInterval <= 0) {
				tickInterval = 10000;
			}

			Condition* condition = getDamageCondition(CONDITION_DAZZLED, damage, damage, 0, tickInterval);
			combat->addCondition(condition);

			return this;
		}

		CombatBuilder* withConditionCurse(int32_t damage, uint32_t tickInterval = 4000)
		{
			if (tickInterval <= 0) {
				tickInterval = 4000;
			}

			Condition* condition = getDamageCondition(CONDITION_CURSED, damage, damage, 0, tickInterval);
			combat->addCondition(condition);

			return this;
		}

		CombatBuilder* withConditionBleed(int32_t damage, uint32_t tickInterval = 4000)
		{
			if (tickInterval <= 0) {
				tickInterval = 4000;
			}

			Condition* condition = getDamageCondition(CONDITION_POISON, damage, damage, 0, tickInterval);
			combat->addCondition(condition);

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
				max = min; // static speedchange value
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
			Condition* condition = Condition::createCondition(CONDITIONID_COMBAT, CONDITION_INVISIBLE, duration, 0);
			combat->setParam(COMBAT_PARAM_AGGRESSIVE, 0);
			combat->addCondition(condition);
			return this;
		}
	};
};

// Lua reference data struct
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

#endif
