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
damage_condition_params getDamageConditionParamsFromXML(const pugi::xml_node node, MonsterSpell* spell)
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

MonsterSpell::MonsterSpell(const std::string& name, const std::string& scriptName) : name(name), scriptName(scriptName)
{
	if (!scriptName.empty()) {
		isScripted = true;
	}
	isScripted = false;
}

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
	this->setRange(1);

	if (attack > 0 && skill > 0) {
		this->setMaxCombatValue(-Weapons::getMaxMeleeDamage(skill, attack));
		this->setMinCombatValue(0);
	}

	return this;
}

MonsterSpell* MonsterSpell::Factory::loadFromXMLNode(pugi::xml_node node, bool reloading)
{
	pugi::xml_attribute attr;
	auto mSpell = new MonsterSpell(node.attribute("name").as_string(), node.attribute("script").as_string());
	if (mSpell->scriptName.empty() && mSpell->name.empty()) {
		std::cout << "[Warning - MonstersSpell::loadFromXMLNode] missing name or script name for spell" << std::endl;

		return nullptr;
	}

	if ((attr = node.attribute("speed")) || (attr = node.attribute("interval"))) {
		mSpell->setSpeed(pugi::cast<int32_t>(attr.value()));
	}

	if ((attr = node.attribute("chance"))) {
		mSpell->setChance(pugi::cast<uint32_t>(attr.value()));
	} else if (boost::algorithm::to_lower_copy(mSpell->getName()) != "melee") {
		std::cout << "[Warning - MonstersSpell::loadFromXMLNode] Missing chance value on non-melee spell: "
		          << mSpell->getName() << std::endl;
	}

	if ((attr = node.attribute("range"))) {
		uint32_t range = pugi::cast<uint32_t>(attr.value());
		mSpell->setRange(range);
	}

	if ((attr = node.attribute("max"))) {
		mSpell->setMaxCombatValue(pugi::cast<int32_t>(attr.value()));
	}

	if ((attr = node.attribute("min"))) {
		mSpell->setMinCombatValue(pugi::cast<int32_t>(attr.value()));
	}

	if (auto spell = g_spells->getSpellByName(mSpell->getName())) {
		mSpell->setBaseSpell(spell);
		return mSpell;
	}

	if (mSpell->isScripted) {
		bool needTarget = false;
		bool needDirection = false;

		if ((attr = node.attribute("direction"))) {
			needDirection = attr.as_bool();
		}

		if ((attr = node.attribute("target"))) {
			needTarget = attr.as_bool();
		}

		return mSpell->setSpellFromScript(needTarget, needDirection);
	}

	auto combatBuilder = std::make_unique<MonsterSpell::CombatBuilder>();
	if ((attr = node.attribute("length"))) {
		int32_t length = pugi::cast<int32_t>(attr.value());
		int32_t spread = 3;
		// need direction spell
		if ((attr = node.attribute("spread"))) {
			spread = pugi::cast<int32_t>(attr.value());
		}

		combatBuilder->withLength(length, spread);
	}

	if ((attr = node.attribute("radius"))) {
		bool needTarget = false;
		int32_t radius = pugi::cast<int32_t>(attr.value());

		// target spell
		if ((attr = node.attribute("target"))) {
			needTarget = attr.as_bool();
		}

		combatBuilder->withRadius(radius, needTarget);
	}

	if ((attr = node.attribute("ring"))) {
		bool needTarget = false;
		int32_t ring = pugi::cast<int32_t>(attr.value());

		// target spell
		if ((attr = node.attribute("target"))) {
			needTarget = attr.as_bool();
		}

		combatBuilder->withRing(ring, needTarget);
	}

	std::string tmpName = boost::algorithm::to_lower_copy(mSpell->getName());
	if (tmpName == "melee") {
		pugi::xml_attribute attackAttribute, skillAttribute;
		if ((attackAttribute = node.attribute("attack")) && (skillAttribute = node.attribute("skill"))) {
			mSpell->setMeleeAttack(pugi::cast<int32_t>(skillAttribute.value()),
			                       pugi::cast<int32_t>(attackAttribute.value()));
		} else {
			mSpell->setMeleeAttack(0, 0);
		}

		uint32_t tickInterval = 0;
		if ((attr = node.attribute("tick"))) {
			int32_t value = pugi::cast<int32_t>(attr.value());
			if (value > 0) {
				tickInterval = value;
			}
		}

		if ((attr = node.attribute("fire"))) {
			combatBuilder->withConditionFire(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("poison"))) {
			combatBuilder->withConditionPoison(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("energy"))) {
			combatBuilder->withConditionEnergy(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("drown"))) {
			combatBuilder->withConditionDrown(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("freeze"))) {
			combatBuilder->withConditionFreeze(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("dazzle"))) {
			combatBuilder->withConditionDazzle(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("curse"))) {
			combatBuilder->withConditionCurse(pugi::cast<int32_t>(attr.value()), tickInterval);
		} else if ((attr = node.attribute("bleed")) || (attr = node.attribute("physical"))) {
			combatBuilder->withConditionBleed(pugi::cast<int32_t>(attr.value()), tickInterval);
		}

		combatBuilder->withMelee();
	} else if (tmpName == "physical") {
		combatBuilder->withPhysicalDamage();
	} else if (tmpName == "bleed") {
		combatBuilder->withBleedDamage();
	} else if (tmpName == "poison" || tmpName == "earth") {
		combatBuilder->withEarthDamage();
	} else if (tmpName == "fire") {
		combatBuilder->withFireDamage();
	} else if (tmpName == "energy") {
		combatBuilder->withEnergyDamage();
	} else if (tmpName == "drown") {
		combatBuilder->withDrownDamage();
	} else if (tmpName == "ice") {
		combatBuilder->withIceDamage();
	} else if (tmpName == "holy") {
		combatBuilder->withHolyDamage();
	} else if (tmpName == "death") {
		combatBuilder->withDeathDamage();
	} else if (tmpName == "lifedrain") {
		combatBuilder->withLifeDrainDamage();
	} else if (tmpName == "manadrain") {
		combatBuilder->withManaDrainDamage();
	} else if (tmpName == "healing") {
		combatBuilder->withHealing();
	} else if (tmpName == "speed") {
		int32_t minSpeedChange = 0;
		int32_t maxSpeedChange = 0;
		int32_t duration = 10000;

		if ((attr = node.attribute("duration"))) {
			duration = pugi::cast<int32_t>(attr.value());
		}

		if ((attr = node.attribute("speedchange"))) {
			minSpeedChange = pugi::cast<int32_t>(attr.value());
		} else if ((attr = node.attribute("minspeedchange")) || (attr = node.attribute("min"))) {
			minSpeedChange = pugi::cast<int32_t>(attr.value());

			if ((attr = node.attribute("maxspeedchange")) || (attr = node.attribute("max"))) {
				maxSpeedChange = pugi::cast<int32_t>(attr.value());
			}
		} else {
			std::cout << "[Error - MonsterSpell::loadFromXMLNode] - " << mSpell->getName()
			          << " - missing speedchange/minspeedchange value" << std::endl;
			return nullptr;
		}

		combatBuilder->withSpeedChange(minSpeedChange, maxSpeedChange, duration);
	} else if (tmpName == "outfit") {
		int32_t duration = 10000;

		if ((attr = node.attribute("duration"))) {
			duration = pugi::cast<int32_t>(attr.value());
		}

		if ((attr = node.attribute("monster"))) {
			const auto& mType = g_monsters.findMonsterTypeByName(attr.as_string());
			if (!mType->isUndefined()) {
				combatBuilder->withOutfitChange(mType->info.outfit, duration);
			}
		} else if ((attr = node.attribute("item"))) {
			auto lookTypeEx = pugi::cast<uint16_t>(attr.value());
			combatBuilder->withOutfitChange(lookTypeEx, duration);
		}
	} else if (tmpName == "invisible") {
		int32_t duration = 10000;

		if ((attr = node.attribute("duration"))) {
			duration = pugi::cast<int32_t>(attr.value());
		}

		combatBuilder->withInvisibility(duration);
	} else if (tmpName == "drunk") {
		int32_t duration = 10000;
		uint8_t drunkenness = 25;

		if ((attr = node.attribute("duration"))) {
			duration = pugi::cast<int32_t>(attr.value());
		}

		if ((attr = node.attribute("drunkenness"))) {
			drunkenness = pugi::cast<uint8_t>(attr.value());
		}

		combatBuilder->withDrunk(duration, drunkenness);
	} else if (tmpName == "firefield") {
		combatBuilder->withFireField();
	} else if (tmpName == "poisonfield") {
		combatBuilder->withPoisonField();
	} else if (tmpName == "energyfield") {
		combatBuilder->withEnergyField();
	} else if (tmpName == "firecondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, mSpell);
		combatBuilder->withConditionFire(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                 conditionParams.startDamage);
	} else if (tmpName == "poisoncondition" || tmpName == "earthcondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, mSpell);
		combatBuilder->withConditionPoison(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                   conditionParams.startDamage);
	} else if (tmpName == "energycondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, mSpell);
		combatBuilder->withConditionEnergy(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                   conditionParams.startDamage);
	} else if (tmpName == "drowncondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, mSpell);
		combatBuilder->withConditionDrown(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                  conditionParams.startDamage);
	} else if (tmpName == "freezecondition" || tmpName == "icecondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, mSpell);
		combatBuilder->withConditionFreeze(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                   conditionParams.startDamage);
	} else if (tmpName == "cursecondition" || tmpName == "deathcondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, mSpell);
		combatBuilder->withConditionCurse(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                  conditionParams.startDamage);
	} else if (tmpName == "dazzlecondition" || tmpName == "holycondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, mSpell);
		combatBuilder->withConditionDazzle(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                   conditionParams.startDamage);
	} else if (tmpName == "physicalcondition" || tmpName == "bleedcondition") {
		auto conditionParams = getDamageConditionParamsFromXML(node, mSpell);
		combatBuilder->withConditionBleed(conditionParams.minDamage, conditionParams.maxDamage, conditionParams.tick,
		                                  conditionParams.startDamage);
	} else if (tmpName == "strength") {
		//
	} else if (tmpName == "effect") {
		//
	} else {
		std::cout << "[Error - MonsterSpell::loadFromXMLNode] - " << mSpell->getName()
		          << " - Unknown spell name: " << tmpName << std::endl;
		return nullptr;
	}

	for (auto attrNode : node.children()) {
		if (auto attr = attrNode.attribute("key")) {
			const auto value = attr.value();
			if (caseInsensitiveEqual(value, "shooteffect")) {
				if (attr = attrNode.attribute("value")) {
					ShootType_t shoot = getShootType(boost::algorithm::to_lower_copy<std::string>(attr.as_string()));
					combatBuilder->withShootType(shoot);
				}
			} else if (caseInsensitiveEqual(value, "areaeffect")) {
				if (attr = attrNode.attribute("value")) {
					MagicEffectClasses effect =
					    getMagicEffect(boost::algorithm::to_lower_copy<std::string>(attr.as_string()));
					combatBuilder->withMagicEffect(effect);
				}
			} else {
				std::cout << "[Warning - MonsterSpell::loadFromXMLNode] - " << mSpell->getName() << " - Effect type: \""
				          << attr.as_string() << "\" does not exist." << std::endl;
			}
		}
	}

	auto combatPtr = combatBuilder->build();
	combatPtr->setPlayerCombatValues(COMBAT_FORMULA_DAMAGE, mSpell->minCombatValue, 0, mSpell->maxCombatValue, 0);

	auto combatSpell = new CombatSpell(combatPtr, combatBuilder->isTargetNeeded(), combatBuilder->isDirectionNeeded());
	mSpell->setBaseSpell(combatSpell);
	if (combatSpell) {
		mSpell->combatSpell = true;
	}

	return mSpell;
}

std::shared_ptr<MonsterSpell> MonsterSpell::Factory::deserializeSpellFromLua(LMonsterSpell* lSpell)
{
	auto mSpell = std::make_shared<MonsterSpell>(lSpell->name, lSpell->scriptName);
	if (mSpell->scriptName.empty() && mSpell->name.empty()) {
		std::cout << "[Warning - MonstersSpell::deserializeSpellFromLua] missing name or script name for spell"
		          << std::endl;

		return nullptr;
	}

	mSpell->setSpeed(lSpell->interval)
	    ->setChance(lSpell->chance)
	    ->setRange(lSpell->range)
	    ->setMaxCombatValue(lSpell->maxCombatValue)
	    ->setMinCombatValue(lSpell->minCombatValue);

	if (auto spell = g_spells->getSpellByName(mSpell->name)) {
		mSpell->setBaseSpell(spell);
		return mSpell;
	}

	if (mSpell->isScripted) {
		mSpell->setSpellFromScript(lSpell->needTarget, lSpell->needDirection);
		return mSpell;
	}

	mSpell->combatSpell = true;
	auto combatBuilder = std::make_unique<MonsterSpell::CombatBuilder>();
	combatBuilder->withLength(lSpell->length, lSpell->spread)
	    ->withRadius(lSpell->radius, lSpell->needTarget)
	    ->withRing(lSpell->ring, lSpell->needTarget)
	    ->withMagicEffect(lSpell->effect)
	    ->withCondition(lSpell->conditionType, std::abs(lSpell->conditionMinDamage),
	                    std::abs(lSpell->conditionMaxDamage), lSpell->tickInterval,
	                    std::abs(lSpell->conditionStartDamage));

	std::string tmpName = boost::algorithm::to_lower_copy(lSpell->name);
	if (tmpName == "melee") {
		mSpell->setMeleeAttack(lSpell->attack, lSpell->skill);
		combatBuilder->withMelee();
	} else if (tmpName == "combat") {
		switch (lSpell->combatType) {
			case COMBAT_UNDEFINEDDAMAGE:
				std::cout << "[Warning - MonsterSpell::deserializeSpellFromLua] - " << tmpName
				          << " - spell has undefined damage" << std::endl;
				combatBuilder->withBleedDamage();
				break;
			case COMBAT_PHYSICALDAMAGE:
				combatBuilder->withPhysicalDamage();
				break;
			case COMBAT_HEALING:
				combatBuilder->withHealing();
				break;
			default:
				combatBuilder->withCombatType(lSpell->combatType);
				break;
		}
	} else if (tmpName == "speed") {
		combatBuilder->withSpeedChange(lSpell->minSpeedChange, lSpell->maxSpeedChange, lSpell->duration);
	} else if (tmpName == "outfit") {
		combatBuilder->withOutfitChange(lSpell->outfit, lSpell->duration);
	} else if (tmpName == "invisible") {
		combatBuilder->withInvisibility(lSpell->duration);
	} else if (tmpName == "drunk") {
		combatBuilder->withDrunk(lSpell->duration, lSpell->drunkenness);
	} else if (tmpName == "firefield") {
		combatBuilder->withFireField();
	} else if (tmpName == "poisonfield") {
		combatBuilder->withPoisonField();
	} else if (tmpName == "energyfield") {
		combatBuilder->withEnergyField();
	} else {
		std::cout << "[Error - MonsterSpell::deserializeSpellFromLua] - " << tmpName << " - Unknown spell name";
	}

	if (combatBuilder->isTargetNeeded()) {
		combatBuilder->withShootType(lSpell->shoot);
	}

	auto combat = combatBuilder->build();
	combat->setPlayerCombatValues(COMBAT_FORMULA_DAMAGE, mSpell->minCombatValue, 0, mSpell->maxCombatValue, 0);

	auto combatSpell = new CombatSpell(combat, combatBuilder->isTargetNeeded(), combatBuilder->isDirectionNeeded());
	mSpell->setBaseSpell(combatSpell);
	if (combatSpell) {
		mSpell->combatSpell = true;
	}

	return mSpell;
}
