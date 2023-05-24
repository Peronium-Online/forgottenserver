#include "libs/monster/MonsterType.h"

#include "libs/monster/Monsters.h"

extern Monsters g_monsters;
MonsterType MonsterType::UNDEFINED_MONSTER_TYPE;

MonsterType* MonsterType::Builder::build()
{
	if (this->mType->info.manaCost == 0 && (this->mType->info.isSummonable || this->mType->info.isConvinceable)) {
		std::cout
		    << "[Warning - MonsterType::build] manaCost missing or zero on monster with summonable and/or convinceable flags: "
		    << filepath << std::endl;
	}

	if (this->mType->info.health > this->mType->info.healthMax) {
		std::cout << "[Warning - MonsterType::build] Health now is greater than health max." << filepath << std::endl;
	}

	this->mType->info.summons.shrink_to_fit();
	this->mType->info.lootItems.shrink_to_fit();
	this->mType->info.attackSpells.shrink_to_fit();
	this->mType->info.defenseSpells.shrink_to_fit();
	this->mType->info.voice.shrinkToFit();
	this->mType->info.scripts.shrink_to_fit();
	this->mType->info.seeks.shrink_to_fit();

	return this->mType;
}

bool MonsterType::Builder::loadRootNodeAttributes(pugi::xml_node node, bool reloading)
{
	pugi::xml_attribute attr;
	if (!(attr = node.attribute("name"))) {
		std::cout << "[Error - MonsterType::loadRootNodeAttributes] Missing name in: " << this->filepath << std::endl;
		return false;
	}

	const std::string& monsterName = attr.as_string();

	if (reloading) {
		auto it = g_monsters.findMonsterTypeByName(monsterName);
		if (!it->isUndefined()) {
			mType = it;
			mType->info = {};
		}
	}

	this->setName(monsterName);
	if (attr = node.attribute("nameDescription")) {
		this->setNameDescription(attr.as_string());
	}

	if ((attr = node.attribute("race"))) {
		std::string tmpStrValue = boost::algorithm::to_lower_copy<std::string>(attr.as_string());
		uint16_t tmpInt = pugi::cast<uint16_t>(attr.value());

		this->setRace(tmpStrValue);
		this->setRace(tmpInt);
	}

	if (attr = node.attribute("experience")) {
		this->setExperience(pugi::cast<uint64_t>(attr.value()));
	}

	if (attr = node.attribute("speed")) {
		this->setSpeed(pugi::cast<int32_t>(attr.value()));
	}

	if (attr = node.attribute("manacost")) {
		this->setManaCost(pugi::cast<uint32_t>(attr.value()));
	}

	if (attr = node.attribute("skull")) {
		this->setSkull(attr.as_string());
	}

	if (attr = node.attribute("script")) {
		this->setScript(attr.as_string());
	}

	return true;
}

bool MonsterType::Builder::load(pugi::xml_node node, bool reloading)
{
	pugi::xml_attribute attr;
	std::string name = boost::algorithm::to_lower_copy(std::string(node.name()));

	if (name == "health") {
		if (attr = node.attribute("now")) {
			this->setHealthNow(pugi::cast<int32_t>(attr.value()));
		} else {
			std::cout << "[Error - MonsterType::load] Missing health now. " << filepath << std::endl;
			return false;
		}

		if (attr = node.attribute("max")) {
			this->setHealthMax(pugi::cast<int32_t>(attr.value()));
		} else {
			std::cout << "[Error - MonsterType::load] Missing health max. " << filepath << std::endl;
			return false;
		}

		return true;
	}

	if (name == "flags") {
		for (auto flagNode : node.children()) {
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
				std::cout << "[Warning - MonsterType::load] Unknown flag attribute: " << attrName << ". " << filepath
				          << " node: " << name << std::endl;
			}
		}

		return true;
	}

	if (name == "targetchange") {
		uint32_t interval = 2000;
		if ((attr = node.attribute("speed")) || (attr = node.attribute("interval"))) {
			interval = pugi::cast<uint32_t>(attr.value());
		} else {
			std::cout << "[Warning - MonsterType::load] Missing targetchange speed. " << filepath << " node: " << name
			          << std::endl;
		}

		int32_t chance = 0;
		if ((attr = node.attribute("chance"))) {
			mType->info.changeTargetChance = pugi::cast<int32_t>(attr.value());
		} else {
			std::cout << "[Warning - MonsterType::load] Missing targetchange chance. " << filepath << " node: " << name
			          << std::endl;
		}

		this->setTargetChange(interval, chance);
		return true;
	}

	if (name == "look") {
		Outfit_t outfit = {};

		if ((attr = node.attribute("type"))) {
			outfit.lookType = pugi::cast<uint16_t>(attr.value());

			if ((attr = node.attribute("head"))) {
				outfit.lookHead = pugi::cast<uint16_t>(attr.value());
			}

			if ((attr = node.attribute("body"))) {
				outfit.lookBody = pugi::cast<uint16_t>(attr.value());
			}

			if ((attr = node.attribute("legs"))) {
				outfit.lookLegs = pugi::cast<uint16_t>(attr.value());
			}

			if ((attr = node.attribute("feet"))) {
				outfit.lookFeet = pugi::cast<uint16_t>(attr.value());
			}

			if ((attr = node.attribute("addons"))) {
				outfit.lookAddons = pugi::cast<uint16_t>(attr.value());
			}
		} else if ((attr = node.attribute("typeex"))) {
			outfit.lookTypeEx = pugi::cast<uint16_t>(attr.value());
		} else {
			std::cout << "[Warning - MonsterType::load] Missing look type/typeex. " << filepath << " node: " << name
			          << std::endl;
		}

		if ((attr = node.attribute("mount"))) {
			outfit.lookMount = pugi::cast<uint16_t>(attr.value());
		}

		uint16_t corpse = 0;
		if ((attr = node.attribute("corpse"))) {
			corpse = pugi::cast<uint16_t>(attr.value());
		}

		this->setLook(outfit, corpse);
		return true;
	}

	if (name == "attacks") {
		for (auto attackNode : node.children()) {
			auto spell = MonsterSpell::Factory().loadFromXMLNode(attackNode, reloading);
			if (spell) {
				this->addAttackSpell(std::move(*spell));
			} else {
				std::cout << "[Warning - MonsterType::load] Cant load spell. " << filepath << " node: " << name
				          << std::endl;
			}
		}

		return true;
	}

	if (name == "defenses") {
		if (attr = node.attribute("defense")) {
			this->setDefense(pugi::cast<int32_t>(attr.value()));
		}

		if (attr = node.attribute("armor")) {
			this->setArmor(pugi::cast<int32_t>(attr.value()));
		}

		for (auto defenseNode : node.children()) {
			auto spell = MonsterSpell::Factory().loadFromXMLNode(defenseNode, reloading);
			if (spell) {
				this->addDefenseSpell(std::move(*spell));
			} else {
				std::cout << "[Warning - MonsterType::load] Cant load spell. " << filepath << " node: " << name
				          << std::endl;
			}
		}

		return true;
	}

	if (name == "immunities") {
		for (auto immunityNode : node.children()) {
			if (attr = immunityNode.attribute("name")) {
				std::string tmpStrValue = boost::algorithm::to_lower_copy<std::string>(attr.as_string());
				if (tmpStrValue == "physical") {
					this->withImmunityToPhysical();
				} else if (tmpStrValue == "energy") {
					this->withImmunityToEnergy();
				} else if (tmpStrValue == "fire") {
					this->withImmunityToFire();
				} else if (tmpStrValue == "poison" || tmpStrValue == "earth") {
					this->withImmunityToPoison();
				} else if (tmpStrValue == "drown") {
					this->withImmunityToDrown();
				} else if (tmpStrValue == "ice") {
					this->withImmunityToIce();
				} else if (tmpStrValue == "holy") {
					this->withImmunityToHoly();
				} else if (tmpStrValue == "death") {
					this->withImmunityToDeath();
				} else if (tmpStrValue == "lifedrain") {
					this->withImmunityToLifeDrain();
				} else if (tmpStrValue == "manadrain") {
					this->withImmunityToManaDrain();
				} else if (tmpStrValue == "paralyze") {
					this->withImmunityToParalyze();
				} else if (tmpStrValue == "outfit") {
					this->withImmunityToOutfitChange();
				} else if (tmpStrValue == "drunk") {
					this->withImmunityToDrunk();
				} else if (tmpStrValue == "invisible" || tmpStrValue == "invisibility") {
					this->withImmunityToInvisibility();
				} else if (tmpStrValue == "bleed") {
					this->withImmunityToBleed();
				} else {
					std::cout << "[Warning - MonsterType::load] Unknown immunity name " << attr.as_string() << ". "
					          << filepath << " node: " << name << std::endl;
				}
			} else if ((attr = immunityNode.attribute("physical"))) {
				if (attr.as_bool()) {
					this->withImmunityToPhysical();
				}
			} else if ((attr = immunityNode.attribute("energy"))) {
				if (attr.as_bool()) {
					this->withImmunityToEnergy();
				}
			} else if ((attr = immunityNode.attribute("fire"))) {
				if (attr.as_bool()) {
					this->withImmunityToFire();
				}
			} else if ((attr = immunityNode.attribute("poison")) || (attr = immunityNode.attribute("earth"))) {
				if (attr.as_bool()) {
					this->withImmunityToPoison();
				}
			} else if ((attr = immunityNode.attribute("drown"))) {
				if (attr.as_bool()) {
					this->withImmunityToDrown();
				}
			} else if ((attr = immunityNode.attribute("ice"))) {
				if (attr.as_bool()) {
					this->withImmunityToIce();
				}
			} else if ((attr = immunityNode.attribute("holy"))) {
				if (attr.as_bool()) {
					this->withImmunityToHoly();
				}
			} else if ((attr = immunityNode.attribute("death"))) {
				if (attr.as_bool()) {
					this->withImmunityToDeath();
				}
			} else if ((attr = immunityNode.attribute("lifedrain"))) {
				if (attr.as_bool()) {
					this->withImmunityToLifeDrain();
				}
			} else if ((attr = immunityNode.attribute("manadrain"))) {
				if (attr.as_bool()) {
					this->withImmunityToManaDrain();
				}
			} else if ((attr = immunityNode.attribute("paralyze"))) {
				if (attr.as_bool()) {
					this->withImmunityToParalyze();
				}
			} else if ((attr = immunityNode.attribute("outfit"))) {
				if (attr.as_bool()) {
					this->withImmunityToOutfitChange();
				}
			} else if ((attr = immunityNode.attribute("bleed"))) {
				if (attr.as_bool()) {
					this->withImmunityToBleed();
				}
			} else if ((attr = immunityNode.attribute("drunk"))) {
				if (attr.as_bool()) {
					this->withImmunityToDrunk();
				}
			} else if ((attr = immunityNode.attribute("invisible")) ||
			           (attr = immunityNode.attribute("invisibility"))) {
				if (attr.as_bool()) {
					this->withImmunityToInvisibility();
				}
			} else {
				std::cout << "[Warning - MonsterType::load] Unknown immunity name " << attr.as_string() << ". "
				          << filepath << " node: " << name << std::endl;
			}
		}

		return true;
	}

	if (name == "voices") {
		MonsterVoice voice;
		if ((attr = node.attribute("speed")) || (attr = node.attribute("interval"))) {
			voice.setYellSpeedTicks(pugi::cast<uint32_t>(attr.value()));
		} else {
			std::cout << "[Warning - MonsterType::load] Missing voices speed. " << filepath << " node: " << name
			          << std::endl;
		}

		if (attr = node.attribute("chance")) {
			voice.setYellChance(pugi::cast<uint32_t>(attr.value()));
		} else {
			std::cout << "[Warning - MonsterType::load] Missing voices chance. " << filepath << " node: " << name
			          << std::endl;
		}

		for (auto voiceNode : node.children()) {
			std::string sentence;
			if (attr = voiceNode.attribute("sentence")) {
				sentence = attr.as_string();
			} else {
				std::cout << "[Warning - MonsterType::load]  Missing voice sentence. " << filepath << " node: " << name
				          << std::endl;
			}

			bool yell = false;
			if (attr = voiceNode.attribute("yell")) {
				yell = attr.as_bool();
			}

			voice.addVoiceBlock(sentence, yell);
		}

		this->setVoice(voice);

		return true;
	}

	if (name == "loot") {
		for (auto lootNode : node.children()) {
			MonsterLoot monsterLoot;
			if (monsterLoot.loadFromXMLNode(lootNode, reloading)) {
				this->addLoot(std::move(monsterLoot));
			} else {
				std::cout << "[Warning - MonsterType::load] Cant load loot. " << filepath << " node: " << name
				          << std::endl;
			}
		}

		return true;
	}

	if (name == "elements") {
		for (auto elementNode : node.children()) {
			if (attr = elementNode.attribute("physicalPercent")) {
				this->withImmunityToPhysical(pugi::cast<uint32_t>(attr.value()));
			} else if (attr = elementNode.attribute("energyPercent")) {
				this->withImmunityToEnergy(pugi::cast<uint32_t>(attr.value()));
			} else if (attr = elementNode.attribute("firePercent")) {
				this->withImmunityToFire(pugi::cast<uint32_t>(attr.value()));
			} else if ((attr = elementNode.attribute("poisonPercent")) ||
			           (attr = elementNode.attribute("earthPercent"))) {
				this->withImmunityToPoison(pugi::cast<uint32_t>(attr.value()));
			} else if (attr = elementNode.attribute("drownPercent")) {
				this->withImmunityToDrown(pugi::cast<uint32_t>(attr.value()));
			} else if (attr = elementNode.attribute("icePercent")) {
				this->withImmunityToIce(pugi::cast<uint32_t>(attr.value()));
			} else if (attr = elementNode.attribute("holyPercent")) {
				this->withImmunityToHoly(pugi::cast<uint32_t>(attr.value()));
			} else if (attr = elementNode.attribute("deathPercent")) {
				this->withImmunityToDeath(pugi::cast<uint32_t>(attr.value()));
			} else if (attr = elementNode.attribute("lifedrainPercent")) {
				this->withImmunityToLifeDrain(pugi::cast<uint32_t>(attr.value()));
			} else if (attr = elementNode.attribute("manadrainPercent")) {
				this->withImmunityToManaDrain(pugi::cast<uint32_t>(attr.value()));
			} else {
				std::cout << "[Warning - MonsterType::load] Unknown element percent. " << filepath << " node: " << name
				          << std::endl;
			}
		}

		return true;
	}

	if (name == "summons") {
		if (attr = node.attribute("maxSummons")) {
			this->setMaxSummons(pugi::cast<uint32_t>(attr.value()));
		} else {
			std::cout << "[Warning - MonsterType::load] Missing summons maxSummons. " << filepath << " node: " << name
			          << std::endl;
		}

		for (auto summonNode : node.children()) {
			if (attr = summonNode.attribute("name")) {
				MonsterSummon* summon = new MonsterSummon(attr.as_string());

				if (attr = summonNode.attribute("chance")) {
					summon->setChance(pugi::cast<int32_t>(attr.value()));
				}

				if ((attr = summonNode.attribute("speed")) || (attr = summonNode.attribute("interval"))) {
					summon->setSpeed(pugi::cast<int32_t>(attr.value()));
				}

				if (attr = summonNode.attribute("max")) {
					summon->setMax(pugi::cast<uint32_t>(attr.value()));
				}

				if (attr = summonNode.attribute("force")) {
					summon->setForce(attr.as_bool());
				}

				this->addSummon(*summon);
			} else {
				std::cout << "[Warning - MonsterType::load] Missing summon name. " << filepath << " node: " << name
				          << std::endl;
			}
		}

		return true;
	}

	if (name == "seeks") {
		for (auto seekNode : node.children()) {
			if (attr = seekNode.attribute("name")) {
				MonsterSeek* seek = new MonsterSeek(attr.as_string());

				if (attr = seekNode.attribute("priority")) {
					seek->setPriority(pugi::cast<uint32_t>(attr.value()));
				}

				this->addSeek(*seek);
			} else {
				std::cout << "[Warning - MonsterType::load] Missing target to seek name. " << filepath
				          << " node: " << name << std::endl;
			}
		}

		return true;
	}

	if (name == "script") {
		for (auto scriptNode : node.children()) {
			if (attr = scriptNode.attribute("name")) {
				this->addEvent(attr.as_string());
			} else {
				std::cout << "[Warning - MonsterType::load] Missing name for script event. " << filepath
				          << " node: " << name << std::endl;
			}
		}

		return true;
	}

	std::cout << "[Error - MonsterType::load] Unexpected node name at " << filepath << " node: " << name << std::endl;
	return false;
}

MonsterType::Builder* MonsterType::Builder::setScript(const std::string& filename)
{
	g_monsters.setMonsterTypeScript(*(this->mType), filename);

	return this;
}

bool MonsterType::loadCallback(LuaScriptInterface* scriptInterface)
{
	int32_t id = scriptInterface->getEvent();
	if (id == -1) {
		std::cout << "[Warning - MonsterType::loadCallback] Event not found. " << std::endl;
		return false;
	}

	info.scriptInterface = scriptInterface;
	if (info.eventType == MONSTERS_EVENT_THINK) {
		info.thinkEvent = id;
	} else if (info.eventType == MONSTERS_EVENT_APPEAR) {
		info.creatureAppearEvent = id;
	} else if (info.eventType == MONSTERS_EVENT_DISAPPEAR) {
		info.creatureDisappearEvent = id;
	} else if (info.eventType == MONSTERS_EVENT_MOVE) {
		info.creatureMoveEvent = id;
	} else if (info.eventType == MONSTERS_EVENT_SAY) {
		info.creatureSayEvent = id;
	}
	return true;
}
