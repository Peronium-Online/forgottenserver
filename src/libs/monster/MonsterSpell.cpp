#include "libs/monster/MonsterSpell.h"

#include "../../weapons.h"
#include "libs/monster/Monsters.h"
#include "libs/util/tools/gamehelpers.h"

#include <boost/algorithm/string.hpp>

extern Spells* g_spells;
extern Monsters g_monsters;

struct damage_condition_params
{
	int32_t tick;
	int32_t startDamage;
	int32_t minDamage;
	int32_t maxDamage;
};

std::string MonsterSpell::getName()
{
	if (!this->name.empty()) {
		return this->name;
	}

	if (!this->scriptName.empty()) {
		return this->scriptName;
	}

	return "UNKNOWN";
}

MonsterSpell* MonsterSpell::setSpeed(int32_t speed)
{
	this->speed = std::max<int32_t>(1, speed);
	return this;
}

MonsterSpell* MonsterSpell::setChance(uint32_t chance)
{
	if (chance > 100) {
		chance = 100;
		std::cout << "[Warning - MonstersSpell::setChance] - Chance value out of bounds for spell: " << getName()
		          << std::endl;
	}

	this->chance = chance;
	return this;
}

MonsterSpell* MonsterSpell::setRange(uint32_t range)
{
	if (range > (Map::maxViewportX * 2)) {
		range = Map::maxViewportX * 2;
	}
	this->range = range;
	return this;
}

MonsterSpell* MonsterSpell::setMinCombatValue(int32_t value)
{
	this->minCombatValue = value;

	if (std::abs(value) > std::abs(this->maxCombatValue)) {
		std::cout << "[Warning - MonstersSpell::setMinCombatValue] value greater than max at: " << getName()
		          << std::endl;
		this->maxCombatValue = value;
	}

	return this;
}

MonsterSpell* MonsterSpell::setMaxCombatValue(int32_t value)
{
	this->maxCombatValue = value;

	if (std::abs(value) < std::abs(this->minCombatValue)) {
		std::cout << "[Warning - MonstersSpell::setMaxCombatValue] value less than min at: " << getName() << std::endl;
		this->maxCombatValue = value;
	}

	return this;
}

MonsterSpell* MonsterSpell::setBaseSpell(BaseSpell* spell)
{
	this->spell = spell;
	return this;
}

MonsterSpell* MonsterSpell::setSpellFromScript(bool needTarget, bool needDirection)
{
	CombatSpell* combatSpell = nullptr;

	std::unique_ptr<CombatSpell> combatSpellPtr(new CombatSpell(nullptr, needTarget, needDirection));
	if (!combatSpellPtr->loadScript("data/" + g_spells->getScriptBaseName() + "/scripts/" + scriptName)) {
		std::cout << "[Warning - MonstersSpell::loadFromXMLNode] unable to find the script: " << scriptName
		          << std::endl;
		return nullptr;
	}

	if (!combatSpellPtr->loadScriptCombat()) {
		std::cout << "[Warning - MonstersSpell::loadFromXMLNode] unable to load script combat: " << scriptName
		          << std::endl;
		return nullptr;
	}

	combatSpell = combatSpellPtr.release();
	combatSpell->getCombat()->setPlayerCombatValues(COMBAT_FORMULA_DAMAGE, this->minCombatValue, 0,
	                                                this->maxCombatValue, 0);

	this->setBaseSpell(combatSpell);
	this->combatSpell = true;

	return this;
}

MonsterSpell* MonsterSpell::setMeleeAttack(int32_t attack, int32_t skill)
{
	this->isMelee = true;

	this->setMinCombatValue(0);
	this->setMaxCombatValue(-Weapons::getMaxMeleeDamage(skill, attack));

	return this;
}

MonsterSpell* MonsterSpell::loadFromXMLNode(pugi::xml_node node, bool reloading)
{
	bool isScripted;

	pugi::xml_attribute attr;
	if ((attr = node.attribute("script"))) {
		this->scriptName = attr.as_string();
		isScripted = true;
	} else if ((attr = node.attribute("name"))) {
		this->name = attr.as_string();
		isScripted = false;
	} else {
		return nullptr;
	}

	if ((attr = node.attribute("speed")) || (attr = node.attribute("interval"))) {
		this->setSpeed(pugi::cast<int32_t>(attr.value()));
	}

	if ((attr = node.attribute("chance"))) {
		this->setChance(pugi::cast<uint32_t>(attr.value()));
	} else if (boost::algorithm::to_lower_copy(name) != "melee") {
		std::cout << "[Warning - MonstersSpell::loadFromXMLNode] Missing chance value on non-melee spell: " << getName()
		          << std::endl;
	}

	if ((attr = node.attribute("range"))) {
		uint32_t range = pugi::cast<uint32_t>(attr.value());
		this->setRange(range);
	}

	if ((attr = node.attribute("min"))) {
		this->setMinCombatValue(pugi::cast<int32_t>(attr.value()));
	}

	if ((attr = node.attribute("max"))) {
		this->setMaxCombatValue(pugi::cast<int32_t>(attr.value()));
	}

	if (auto spell = g_spells->getSpellByName(name)) {
		this->setBaseSpell(spell);
		return this;
	}

	if (isScripted) {
		bool needTarget = false;
		bool needDirection = false;

		if ((attr = node.attribute("direction"))) {
			needDirection = attr.as_bool();
		}

		if ((attr = node.attribute("target"))) {
			needTarget = attr.as_bool();
		}

		return this->setSpellFromScript(needTarget, needDirection);
	}

	auto combatBuilder = MonsterSpell::CombatBuilder();
	if ((attr = node.attribute("length"))) {
		int32_t length = pugi::cast<int32_t>(attr.value());
		int32_t spread = 3;
		// need direction spell
		if ((attr = node.attribute("spread"))) {
			spread = pugi::cast<int32_t>(attr.value());
		}

		combatBuilder.withLength(length, spread);
	}

	if ((attr = node.attribute("radius"))) {
		bool needTarget = false;
		int32_t radius = pugi::cast<int32_t>(attr.value());

		// target spell
		if ((attr = node.attribute("target"))) {
			needTarget = attr.as_bool();
		}

		combatBuilder.withRadius(radius, needTarget);
	}

	if ((attr = node.attribute("ring"))) {
		bool needTarget = false;
		int32_t ring = pugi::cast<int32_t>(attr.value());

		// target spell
		if ((attr = node.attribute("target"))) {
			needTarget = attr.as_bool();
		}

		combatBuilder.withRing(ring, needTarget);
	}

	std::string tmpName = boost::algorithm::to_lower_copy(name);
	if (tmpName == "melee") {
		pugi::xml_attribute attackAttribute, skillAttribute;
		if ((attackAttribute = node.attribute("attack")) && (skillAttribute = node.attribute("skill"))) {
			this->setMeleeAttack(pugi::cast<int32_t>(skillAttribute.value()),
			                     pugi::cast<int32_t>(attackAttribute.value()));
		}

		uint32_t tickInterval = 0;
		if ((attr = node.attribute("tick"))) {
			int32_t value = pugi::cast<int32_t>(attr.value());
			if (value > 0) {
				tickInterval = value;
			}
		}

		if ((attr = node.attribute("fire"))) {
			combatBuilder.withConditionFire(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("poison"))) {
			combatBuilder.withConditionPoison(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("energy"))) {
			combatBuilder.withConditionEnergy(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("drown"))) {
			combatBuilder.withConditionDrown(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("freeze"))) {
			combatBuilder.withConditionFreeze(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("dazzle"))) {
			combatBuilder.withConditionDazzle(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("curse"))) {
			combatBuilder.withConditionCurse(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("bleed")) || (attr = node.attribute("physical"))) {
			combatBuilder.withConditionBleed(pugi::cast<int32_t>(attr.value()), tickInterval);
		}

		combatBuilder.withMelee();
		this->setRange(1);
	} else if (tmpName == "physical") {
		combatBuilder.withPhysicalDamage();
	} else if (tmpName == "bleed") {
		combatBuilder.withBleedDamage();
	} else if (tmpName == "poison" || tmpName == "earth") {
		combatBuilder.withEarthDamage();
	} else if (tmpName == "fire") {
		combatBuilder.withFireDamage();
	} else if (tmpName == "energy") {
		combatBuilder.withEnergyDamage();
	} else if (tmpName == "drown") {
		combatBuilder.withDrownDamage();
	} else if (tmpName == "ice") {
		combatBuilder.withIceDamage();
	} else if (tmpName == "holy") {
		combatBuilder.withHolyDamage();
	} else if (tmpName == "death") {
		combatBuilder.withDeathDamage();
	} else if (tmpName == "lifedrain") {
		combatBuilder.withLifeDrainDamage();
	} else if (tmpName == "manadrain") {
		combatBuilder.withManaDrainDamage();
	} else if (tmpName == "healing") {
		combatBuilder.withHealing();
	} else if (tmpName == "speed") {
		int32_t minSpeedChange = 0;
		int32_t maxSpeedChange = 0;
		int32_t duration = 10000;

		if ((attr = node.attribute("duration"))) {
			duration = pugi::cast<int32_t>(attr.value());
		}

		if ((attr = node.attribute("speedchange"))) {
			minSpeedChange = pugi::cast<int32_t>(attr.value());
		} else if ((attr = node.attribute("minspeedchange"))) {
			minSpeedChange = pugi::cast<int32_t>(attr.value());

			if ((attr = node.attribute("maxspeedchange"))) {
				maxSpeedChange = pugi::cast<int32_t>(attr.value());
			}
		} else {
			std::cout << "[Error - MonsterSpell::loadFromXMLNode] - " << getName()
			          << " - missing speedchange/minspeedchange value" << std::endl;
			return nullptr;
		}

		combatBuilder.withSpeedChange(minSpeedChange, maxSpeedChange, duration);
	} else if (tmpName == "outfit") {
		int32_t duration = 10000;

		if ((attr = node.attribute("duration"))) {
			duration = pugi::cast<int32_t>(attr.value());
		}

		if ((attr = node.attribute("monster"))) {
			MonsterType* mType = g_monsters.findMonsterTypeByName(attr.as_string());
			if (mType) {
				combatBuilder.withOutfitChange(mType->info.outfit, duration);
			}
		} else if ((attr = node.attribute("item"))) {
			auto lookTypeEx = pugi::cast<uint16_t>(attr.value());
			combatBuilder.withOutfitChange(lookTypeEx, duration);
		}
	} else if (tmpName == "invisible") {
		int32_t duration = 10000;

		if ((attr = node.attribute("duration"))) {
			duration = pugi::cast<int32_t>(attr.value());
		}

		combatBuilder.withInvisibility(duration);
	} else if (tmpName == "drunk") {
		int32_t duration = 10000;
		uint8_t drunkenness = 25;

		if ((attr = node.attribute("duration"))) {
			duration = pugi::cast<int32_t>(attr.value());
		}

		if ((attr = node.attribute("drunkenness"))) {
			drunkenness = pugi::cast<uint8_t>(attr.value());
		}

		combatBuilder.withDrunk(duration, drunkenness);
	} else if (tmpName == "firefield") {
		combatBuilder.withFireField();
	} else if (tmpName == "poisonfield") {
		combatBuilder.withPoisonField();
	} else if (tmpName == "energyfield") {
		combatBuilder.withEnergyField();
	} else if (tmpName == "firecondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, this);
		combatBuilder.withConditionFire(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                conditionParams.startDamage);
	} else if (tmpName == "poisoncondition" || tmpName == "earthcondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, this);
		combatBuilder.withConditionPoison(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                  conditionParams.startDamage);
	} else if (tmpName == "energycondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, this);
		combatBuilder.withConditionEnergy(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                  conditionParams.startDamage);
	} else if (tmpName == "drowncondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, this);
		combatBuilder.withConditionDrown(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                 conditionParams.startDamage);
	} else if (tmpName == "freezecondition" || tmpName == "icecondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, this);
		combatBuilder.withConditionFreeze(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                  conditionParams.startDamage);
	} else if (tmpName == "cursecondition" || tmpName == "deathcondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, this);
		combatBuilder.withConditionCurse(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                 conditionParams.startDamage);
	} else if (tmpName == "dazzlecondition" || tmpName == "holycondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, this);
		combatBuilder.withConditionDazzle(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                  conditionParams.startDamage);
	} else if (tmpName == "physicalcondition" || tmpName == "bleedcondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, this);
		combatBuilder.withConditionBleed(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                 conditionParams.startDamage);
	} else if (tmpName == "strength") {
		//
	} else if (tmpName == "effect") {
		//
	} else {
		std::cout << "[Error - MonsterSpell::loadFromXMLNode] - " << getName() << " - Unknown spell name: " << name
		          << std::endl;
		return nullptr;
	}

	for (auto attrNode : node.children()) {
		if (auto attr = attrNode.attribute("key")) {
			const auto value = attr.value();
			if (caseInsensitiveEqual(value, "shooteffect")) {
				if (attr = attrNode.attribute("value")) {
					ShootType_t shoot = getShootType(boost::algorithm::to_lower_copy<std::string>(attr.as_string()));
					if (shoot != CONST_ANI_NONE) {
						combatBuilder.withShootType(shoot);
					} else {
						std::cout << "[Warning - MonsterSpell::loadFromXMLNode] - " << getName()
						          << " - Unknown shootEffect: " << attr.as_string() << std::endl;
					}
				}
			} else if (caseInsensitiveEqual(value, "areaeffect")) {
				if (attr = attrNode.attribute("value")) {
					MagicEffectClasses effect =
					    getMagicEffect(boost::algorithm::to_lower_copy<std::string>(attr.as_string()));
					if (effect != CONST_ME_NONE) {
						combatBuilder.withMagicEffect(effect);
					} else {
						std::cout << "[Warning - MonsterSpell::loadFromXMLNode] - " << getName()
						          << " - Unknown areaEffect: " << attr.as_string() << std::endl;
					}
				}
			} else {
				std::cout << "[Warning - MonsterSpell::loadFromXMLNode] - " << getName() << " - Effect type: \""
				          << attr.as_string() << "\" does not exist." << std::endl;
			}
		}
	}

	auto combatPtr = combatBuilder.build();
	combatPtr->setPlayerCombatValues(COMBAT_FORMULA_DAMAGE, this->minCombatValue, 0, this->maxCombatValue, 0);

	auto combatSpell = new CombatSpell(combatPtr, combatBuilder.isTargetNeeded(), combatBuilder.isDirectionNeeded());
	this->setBaseSpell(combatSpell);
	if (combatSpell) {
		this->combatSpell = true;
	}

	return this;
}

MonsterSpell* MonsterSpell::deserializeSpellFromLua(LMonsterSpell* lSpell)
{
	if (!lSpell->scriptName.empty()) {
		lSpell->isScripted = true;
	} else if (!lSpell->name.empty()) {
		lSpell->isScripted = false;
	} else {
		return nullptr;
	}

	this->setSpeed(lSpell->interval)
	    ->setChance(lSpell->chance)
	    ->setRange(lSpell->range)
	    ->setMinCombatValue(lSpell->minCombatValue)
	    ->setMaxCombatValue(lSpell->maxCombatValue);

	if (auto spell = g_spells->getSpellByName(name)) {
		this->setBaseSpell(spell);
		return this;
	}

	if (lSpell->isScripted) {
		return this->setSpellFromScript(lSpell->needTarget, lSpell->needDirection);
	}

	this->combatSpell = true;
	auto combatBuilder = new MonsterSpell::CombatBuilder();
	combatBuilder->withLength(lSpell->length, lSpell->spread)
	    ->withRadius(lSpell->radius, lSpell->needTarget)
	    ->withRing(lSpell->ring, lSpell->needTarget)
	    ->withCondition(lSpell->conditionType, std::abs(lSpell->conditionMinDamage),
	                    std::abs(lSpell->conditionMaxDamage), lSpell->tickInterval,
	                    std::abs(lSpell->conditionStartDamage));
}

damage_condition_params getDamageConditionParamsFromXML(const pugi::xml_node node, const MonsterSpell* spell)
{
	int32_t tickInterval = -1;
	if (pugi::xml_attribute attr = node.attribute("tick")) {
		int32_t value = pugi::cast<int32_t>(attr.value());
		if (value > 0) {
			tickInterval = value;
		}
	}

	int32_t startDamage = 0;
	if (pugi::xml_attribute attr = node.attribute("start")) {
		int32_t value = std::abs(pugi::cast<int32_t>(attr.value()));
		if (value <= spell->minCombatValue) {
			startDamage = value;
		}
	}

	return damage_condition_params{
		startDamage = startDamage,
		tickInterval = tickInterval,
		minDamage : std::abs(spell->minCombatValue),
		maxDamage : std::abs(spell->maxCombatValue),
	};
}
