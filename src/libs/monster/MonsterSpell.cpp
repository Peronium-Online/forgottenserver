#include "libs/monster/MonsterSpell.h"

#include "../../weapons.h"

#include <boost/algorithm/string.hpp>

extern Spells* g_spells;

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
			spread = std::max<int32_t>(0, pugi::cast<int32_t>(attr.value()));
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
	}

	auto combatSpell = combatBuilder.getInstance();
	this->setBaseSpell(combatSpell);
	this->combatSpell = true;

	return this;
}