#include "libs/monster/MonsterType.h"

#include "libs/monster/Monsters.h"
#include "libs/util/tools/gamehelpers.h"

#include <boost/algorithm/string.hpp>

extern Monsters g_monsters;

MonsterType::Builder* MonsterType::Builder::setName(std::string name)
{
	std::string lowerCasedMonsterName = boost::algorithm::to_lower_copy(name);

	this->mType->name = name;
	this->mType->nameDescription = "a " + lowerCasedMonsterName;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setNameDescription(std::string nameDescription)
{
	this->mType->nameDescription = nameDescription;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setRace(std::string raceName)
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

MonsterType::Builder* MonsterType::Builder::setRace(uint16_t raceNumber)
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

MonsterType::Builder* MonsterType::Builder::setExperience(uint64_t exp)
{
	this->mType->info.experience = exp;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setSpeed(int32_t speed)
{
	this->mType->info.baseSpeed = speed;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setSkull(std::string skull)
{
	this->mType->info.skull = getSkullType(boost::algorithm::to_lower_copy<std::string>(skull));
	return this;
}

MonsterType::Builder* MonsterType::Builder::setScript(std::string filename)
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

MonsterType::Builder* MonsterType::Builder::setHealthNow(int32_t healthNow)
{
	this->mType->info.health = healthNow;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setHealthMax(int32_t healthMax)
{
	this->mType->info.healthMax = healthMax;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setManaCost(uint32_t cost)
{
	this->mType->info.manaCost = cost;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setSummonable(bool summonable)
{
	this->mType->info.isSummonable = summonable;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setAttackable(bool attackable)
{
	this->mType->info.isAttackable = attackable;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setHostile(bool hostile)
{
	this->mType->info.isHostile = hostile;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setIgnoreSpawnBlock(bool ignore)
{
	this->mType->info.isIgnoringSpawnBlock = ignore;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setIllusionable(bool illusionable)
{
	this->mType->info.isIllusionable = illusionable;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setChallengeable(bool challengeable)
{
	this->mType->info.isChallengeable = challengeable;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setConvinceable(bool convinceable)
{
	this->mType->info.isConvinceable = convinceable;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setPushable(bool pushable)
{
	if (pushable) {
		this->mType->info.canPushCreatures = false;
	}
	this->mType->info.pushable = pushable;

	return this;
}

MonsterType::Builder* MonsterType::Builder::setBoss(bool boss)
{
	this->mType->info.isBoss = boss;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setCanPushItems(bool canPushItems)
{
	this->mType->info.canPushItems = canPushItems;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setCanPushCreatures(bool canPush)
{
	if (canPush) {
		this->mType->info.pushable = false;
	}
	this->mType->info.canPushCreatures = canPush;

	return this;
}

MonsterType::Builder* MonsterType::Builder::setStaticAttack(uint32_t staticAttack)
{
	if (staticAttack > 100) {
		std::cout << "[Warning - MonsterType::loadFromXMLNode] staticattack greater than 100. " << this->mType->name
		          << std::endl;
		staticAttack = 100;
	}

	this->mType->info.staticAttackChance = staticAttack;

	return this;
}

MonsterType::Builder* MonsterType::Builder::setLightLevel(uint16_t lightLevel)
{
	this->mType->info.light.level = lightLevel;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setLightColor(uint16_t lightColor)
{
	this->mType->info.light.color = lightColor;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setTargetDistance(int32_t targetDistance)
{
	if (targetDistance < 1) {
		targetDistance = 1;
		std::cout << "[Warning - MonsterType::loadFromXMLNode] targetdistance less than 1. " << this->mType->name
		          << std::endl;
	}

	this->mType->info.targetDistance = targetDistance;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setRunOnHealth(int32_t runAwayHealth)
{
	this->mType->info.runAwayHealth = runAwayHealth;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setHideHealth(bool hideHealth)
{
	this->mType->info.hiddenHealth = hideHealth;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setCanWalkOnEnergy(bool canWalk)
{
	this->mType->info.canWalkOnEnergy = canWalk;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setCanWalkOnFire(bool canWalk)
{
	this->mType->info.canWalkOnFire = canWalk;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setCanWalkOnPoison(bool canWalk)
{
	this->mType->info.canWalkOnPoison = canWalk;
	return this;
}

MonsterType::Builder* MonsterType::Builder::setTargetChange(uint32_t interval, int32_t chance)
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

MonsterType::Builder* MonsterType::Builder::setLook(Outfit_t outfit, uint16_t corpse)
{
	this->mType->info.outfit = outfit;
	this->mType->info.lookcorpse = corpse;

	return this;
}

MonsterType* MonsterType::Builder::build()
{
	if (this->mType->info.manaCost == 0 && (this->mType->info.isSummonable || this->mType->info.isConvinceable)) {
		std::cout
		    << "[Warning - MonsterType::loadFromXMLNode] manaCost missing or zero on monster with summonable and/or convinceable flags: "
		    << this->mType->name << std::endl;
	}

	return this->mType;
}

MonsterType* MonsterType::Builder::loadFromXMLNode(pugi::xml_node node, bool reloading)
{
	pugi::xml_attribute attr;
	if (!(attr = node.attribute("name"))) {
		std::cout << "[Error - MonsterType::loadFromXMLNode] Missing name in: " << node.name() << std::endl;
		return nullptr;
	}
	std::string lowerCasedMonsterName = boost::algorithm::to_lower_copy(std::string(attr.as_string()));

	if (reloading) {
		auto it = g_monsters.findMonsterTypeByName(lowerCasedMonsterName);
		if (it != nullptr) {
			mType = it;
			mType->info = {};
		}
	}

	if (!mType) {
		mType = g_monsters.findMonsterTypeByName(lowerCasedMonsterName);
	}

	this->setName(attr.as_string());
	if ((attr = node.attribute("nameDescription"))) {
		this->setNameDescription(attr.as_string());
	}

	if ((attr = node.attribute("race"))) {
		std::string tmpStrValue = boost::algorithm::to_lower_copy<std::string>(attr.as_string());
		uint16_t tmpInt = pugi::cast<uint16_t>(attr.value());

		this->setRace(tmpStrValue);
		this->setRace(tmpInt);
	}

	if ((attr = node.attribute("experience"))) {
		this->setExperience(pugi::cast<uint64_t>(attr.value()));
	}

	if ((attr = node.attribute("speed"))) {
		this->setSpeed(pugi::cast<int32_t>(attr.value()));
	}

	if ((attr = node.attribute("manacost"))) {
		this->setManaCost(pugi::cast<uint32_t>(attr.value()));
	}

	if ((attr = node.attribute("skull"))) {
		this->setSkull(attr.as_string());
	}

	if ((attr = node.attribute("script"))) {
		this->setScript(attr.as_string());
	}

	if (pugi::xml_node flagsNode = node.child("flags")) {
		for (auto flagNode : flagsNode.children()) {
			attr = flagNode.first_attribute();
			const char* attrName = attr.name();

			if (caseInsensitiveEqual(attrName, "summonable")) {
				this->setSummonable(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "attackable")) {
				this->setAttackable(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "hostile")) {
				this->setHostile(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "ignorespawnblock")) {
				this->setIgnoreSpawnBlock(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "illusionable")) {
				this->setIllusionable(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "challengeable")) {
				this->setChallengeable(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "convinceable")) {
				this->setConvinceable(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "pushable")) {
				this->setPushable(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "isboss")) {
				this->setBoss(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "canpushitems")) {
				this->setCanPushItems(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "canpushcreatures")) {
				this->setCanPushCreatures(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "staticattack")) {
				this->setStaticAttack(pugi::cast<uint32_t>(attr.value()));
			} else if (caseInsensitiveEqual(attrName, "lightlevel")) {
				this->setLightLevel(pugi::cast<uint16_t>(attr.value()));
			} else if (caseInsensitiveEqual(attrName, "lightcolor")) {
				this->setLightColor(pugi::cast<uint16_t>(attr.value()));
			} else if (caseInsensitiveEqual(attrName, "targetdistance")) {
				this->setTargetDistance(pugi::cast<int32_t>(attr.value()));
			} else if (caseInsensitiveEqual(attrName, "runonhealth")) {
				this->setRunOnHealth(pugi::cast<int32_t>(attr.value()));
			} else if (caseInsensitiveEqual(attrName, "hidehealth")) {
				this->setHideHealth(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "canwalkonenergy")) {
				this->setCanWalkOnEnergy(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "canwalkonfire")) {
				this->setCanWalkOnFire(attr.as_bool());
			} else if (caseInsensitiveEqual(attrName, "canwalkonpoison")) {
				this->setCanWalkOnPoison(attr.as_bool());
			} else {
				std::cout << "[Warning - MonsterType::loadFromXMLNode] Unknown flag attribute: " << attrName << ". "
				          << this->mType->name << std::endl;
			}
		}
	}

	if (pugi::xml_node targetChangeNode = node.child("targetchange")) {
		uint32_t interval = 2000;
		if ((attr = targetChangeNode.attribute("speed")) || (attr = targetChangeNode.attribute("interval"))) {
			interval = pugi::cast<uint32_t>(attr.value());
		} else {
			std::cout << "[Warning - MonsterType::loadFromXMLNode] Missing targetchange speed. " << this->mType->name
			          << std::endl;
		}

		int32_t chance = 0;
		if ((attr = targetChangeNode.attribute("chance"))) {
			mType->info.changeTargetChance = pugi::cast<int32_t>(attr.value());
		} else {
			std::cout << "[Warning - MonsterType::loadFromXMLNode] Missing targetchange chance. " << this->mType->name
			          << std::endl;
		}

		this->setTargetChange(interval, chance);
	}

	if (pugi::xml_node lookNode = node.child("look")) {
		Outfit_t outfit = {};

		if ((attr = lookNode.attribute("type"))) {
			outfit.lookType = pugi::cast<uint16_t>(attr.value());

			if ((attr = lookNode.attribute("head"))) {
				outfit.lookHead = pugi::cast<uint16_t>(attr.value());
			}

			if ((attr = lookNode.attribute("body"))) {
				outfit.lookBody = pugi::cast<uint16_t>(attr.value());
			}

			if ((attr = lookNode.attribute("legs"))) {
				outfit.lookLegs = pugi::cast<uint16_t>(attr.value());
			}

			if ((attr = lookNode.attribute("feet"))) {
				outfit.lookFeet = pugi::cast<uint16_t>(attr.value());
			}

			if ((attr = lookNode.attribute("addons"))) {
				outfit.lookAddons = pugi::cast<uint16_t>(attr.value());
			}
		} else if ((attr = lookNode.attribute("typeex"))) {
			outfit.lookTypeEx = pugi::cast<uint16_t>(attr.value());
		} else {
			std::cout << "[Warning - MonsterType::loadFromXMLNode] Missing look type/typeex. " << this->mType->name
			          << std::endl;
		}

		if ((attr = lookNode.attribute("mount"))) {
			outfit.lookMount = pugi::cast<uint16_t>(attr.value());
		}

		uint16_t corpse = 0;
		if ((attr = lookNode.attribute("corpse"))) {
			corpse = pugi::cast<uint16_t>(attr.value());
		}

		this->setLook(outfit, corpse);
	}

	if (pugi::xml_node attacksNode = node.child("attacks")) {
		for (auto attackNode : attacksNode.children()) {
			MonsterSpell spell;
			if (spell.loadFromXMLNode(attackNode, reloading)) {
				this->mType->info.attackSpells.emplace_back(std::move(spell));
			} else {
				std::cout << "[Warning - MonsterType::loadFromXMLNode] Cant load spell. " << this->mType->name
				          << std::endl;
			}
		}
	}

	if (pugi::xml_node defensesNode = node.child("defenses")) {
		if (attr = node.attribute("defense")) {
		}
	}

	return this->build();
}
