#include "libs/monster/MonsterType.h"

#include "libs/monster/Monsters.h"
#include "libs/util/tools/gamehelpers.h"

#include <boost/algorithm/string.hpp>

extern Monsters g_monsters;

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
				this->addAttackSpell(std::move(spell));
			} else {
				std::cout << "[Warning - MonsterType::loadFromXMLNode] Cant load spell. " << this->mType->name
				          << std::endl;
			}
		}
	}

	if (pugi::xml_node defensesNode = node.child("defenses")) {
		if (attr = node.attribute("defense")) {
			this->setDefense(pugi::cast<int32_t>(attr.value()));
		}

		if (attr = node.attribute("armor")) {
			this->setArmor(pugi::cast<int32_t>(attr.value()));
		}

		for (auto defenseNode : defensesNode.children()) {
			MonsterSpell spell;
			if (spell.loadFromXMLNode(defenseNode, reloading)) {
				this->addDefenseSpell(std::move(spell));
			} else {
				std::cout << "[Warning - MonsterType::loadFromXMLNode] Cant load spell. " << this->mType->name
				          << std::endl;
			}
		}
	}

	return this->build();
}
