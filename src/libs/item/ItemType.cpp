#include "libs/item/ItemType.h"

#include "libs/item/Items.h"
#include "libs/item/itemmaps.h"
#include "libs/util/tools/direction.h"
#include "libs/util/tools/pugicast.h"

ItemType* ItemType::loadFromXMLNode(pugi::xml_node node, bool reloading)
{
	uint16_t fromId = 0;
	uint16_t toId = 0;

	pugi::xml_attribute idAttribute = node.attribute("id");
	pugi::xml_attribute fromIdAttribute = node.attribute("fromid");
	pugi::xml_attribute toIdAttribute = node.attribute("toid");
	if (idAttribute) {
		fromId = pugi::cast<uint16_t>(idAttribute.value());
		toId = pugi::cast<uint16_t>(idAttribute.value());
	} else if (fromIdAttribute && toIdAttribute) {
		fromId = pugi::cast<uint16_t>(fromIdAttribute.value());
		toId = pugi::cast<uint16_t>(toIdAttribute.value());
	} else {
		std::cout << "[Warning - ItemType::loadFromXMLNode] No item id found" << std::endl;
		return nullptr;
	}

	for (uint16_t id = fromId; id <= toId; id++) {
		if (id == 0) {
			continue;
		}

		ItemType* iType = Items::getInstance().getItemType(id);
		iType->id = id;

		if (!iType->name.empty()) {
			std::cout << "[Warning - ItemType::loadFromXMLNode] Duplicate item with id: " << id << std::endl;
			continue;
		}

		pugi::xml_attribute attr;
		if (attr = node.attribute("article")) {
			iType->article = attr.as_string();
		}

		if (attr = node.attribute("plural")) {
			iType->pluralName = attr.as_string();
		}

		for (auto attributeNode : node.children()) {
			if (!(attr = attributeNode.attribute("key"))) {
				continue;
			}

			if (!(attr = attributeNode.attribute("value"))) {
				continue;
			}
			std::string key = attributeNode.attribute("key").as_string();
			std::string value = attributeNode.attribute("value").as_string();

			std::string lowerCasedKey = boost::algorithm::to_lower_copy<std::string>(key);
			if (lowerCasedKey == "type") {
				iType->setType(value);
			} else if (lowerCasedKey == "description") {
				iType->description = value;
			} else if (lowerCasedKey == "runespellname") {
				iType->runeSpellName = value;
			} else if (lowerCasedKey == "weight") {
				iType->weight = pugi::cast<uint32_t>(value.c_str());
			} else if (lowerCasedKey == "showcount") {
				iType->showCount = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "armor") {
				iType->armor = pugi::cast<int32_t>(value.c_str());
			} else if (lowerCasedKey == "defense") {
				iType->defense = pugi::cast<int32_t>(value.c_str());
			} else if (lowerCasedKey == "extradef") {
				iType->extraDefense = pugi::cast<int32_t>(value.c_str());
			} else if (lowerCasedKey == "attack") {
				iType->attack = pugi::cast<int32_t>(value.c_str());
			} else if (lowerCasedKey == "attackspeed") {
				iType->setAttackSpeed(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "rotateto") {
				iType->rotateTo = pugi::cast<int32_t>(value.c_str());
			} else if (lowerCasedKey == "moveable" || lowerCasedKey == "movable") {
				iType->moveable = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "blockprojectile") {
				iType->blockProjectile = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "allowpickupable" || lowerCasedKey == "pickupable") {
				iType->allowPickupable = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "forceserialize" || lowerCasedKey == "forcesave") {
				iType->forceSerialize = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "floorchange") {
				iType->setFloorChange(value);
			} else if (lowerCasedKey == "corpsetype") {
				iType->setCorpseType(value);
			} else if (lowerCasedKey == "containersize") {
				iType->maxItems = pugi::cast<uint16_t>(value.c_str());
			} else if (lowerCasedKey == "fluidsource") {
				iType->setFluidSource(value);
			} else if (lowerCasedKey == "readable") {
				iType->canReadText = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "writeable") {
				iType->canWriteText = pugi::cast<bool>(value.c_str());
				iType->canReadText = iType->canWriteText;
			} else if (lowerCasedKey == "maxtextlen") {
				iType->maxTextLen = pugi::cast<uint16_t>(value.c_str());
			} else if (lowerCasedKey == "writeonceitemid") {
				iType->writeOnceItemId = pugi::cast<uint16_t>(value.c_str());
			} else if (lowerCasedKey == "weapontype") {
				iType->setWeaponType(value);
			} else if (lowerCasedKey == "slottype") {
				iType->setSlotType(value);
			} else if (lowerCasedKey == "ammotype") {
				iType->setAmmoType(value);
			} else if (lowerCasedKey == "shoottype") {
				iType->setShootType(value);
			} else if (lowerCasedKey == "effect") {
				iType->setMagicEffect(value);
			} else if (lowerCasedKey == "range") {
				iType->shootRange = pugi::cast<uint16_t>(value.c_str());
			} else if (lowerCasedKey == "stopduration") {
				iType->stopTime = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "decayto") {
				iType->decayTo = pugi::cast<uint32_t>(value.c_str());
			} else if (lowerCasedKey == "transformequipto") {
				iType->transformEquipTo = pugi::cast<uint16_t>(value.c_str());
			} else if (lowerCasedKey == "transformdeequipto") {
				iType->transformDeEquipTo = pugi::cast<uint16_t>(value.c_str());
			} else if (lowerCasedKey == "duration") {
				iType->decayTime = pugi::cast<uint32_t>(value.c_str());
			} else if (lowerCasedKey == "showduration") {
				iType->showDuration = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "charges") {
				iType->charges = pugi::cast<uint32_t>(value.c_str());
			} else if (lowerCasedKey == "showcharges") {
				iType->showCharges = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "showattributes") {
				iType->showAttributes = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "hitchance") {
				iType->setHitChance(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "maxhitchance") {
				iType->setMaxHitChance(pugi::cast<uint32_t>(value.c_str()));
			} else if (lowerCasedKey == "invisible") {
				iType->abilities->invisible = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "speed") {
				iType->abilities->speed = pugi::cast<int32_t>(value.c_str());
			} else if (lowerCasedKey == "healthgain") {
				iType->setHealthGain(pugi::cast<uint32_t>(value.c_str()));
			} else if (lowerCasedKey == "healthticks") {
				iType->setHealthTicks(pugi::cast<uint32_t>(value.c_str()));
			} else if (lowerCasedKey == "managain") {
				iType->setManaGain(pugi::cast<uint32_t>(value.c_str()));
			} else if (lowerCasedKey == "manaticks") {
				iType->setManaTicks(pugi::cast<uint32_t>(value.c_str()));
			} else if (lowerCasedKey == "manashield") {
				iType->abilities->manaShield = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "skillsword") {
				iType->setSwordSkill(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "skillaxe") {
				iType->setAxeSkill(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "skillclub") {
				iType->setClubSkill(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "skilldist") {
				iType->setDistSkill(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "skillfish") {
				iType->setFishSkill(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "skillshield") {
				iType->setShieldSkill(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "skillfist") {
				iType->setFistSkill(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "maxhitpoints") {
				iType->setMaxHitPoints(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "maxhitpointspercent") {
				iType->setMaxHitPointsPercent(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "maxmanapoints") {
				iType->setMaxManaPoints(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "maxmanapointspercent") {
				iType->setMaxManaPointsPercent(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "magicpoints" || lowerCasedKey == "magiclevelpoints") {
				iType->setMagicPoints(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "magicpointspercent") {
				iType->setMagicPointsPercent(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "criticalhitchance") {
				iType->setCriticalHitChance(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "criticalhitamount") {
				iType->setCriticalHitAmount(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "lifeleechchance") {
				iType->setLifeLeechChance(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "lifeleechamount") {
				iType->setLifeLeechAmount(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "manaleechchance") {
				iType->setManaLeechChance(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "manaleechamount") {
				iType->setManaLeechAmount(pugi::cast<int32_t>(value.c_str()));
			} else if (lowerCasedKey == "fieldabsorbpercentenergy") {
				iType->setFieldAbsorbPercentEnergy(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "fieldabsorbpercentfire") {
				iType->setFieldAbsorbPercentFire(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "fieldabsorbpercentpoison" || lowerCasedKey == "fieldabsorbpercentearth") {
				iType->setFieldAbsorbPercentEarth(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentall") {
				iType->setAbsorbPercent(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentallelements" || lowerCasedKey == "absorbpercentelements") {
				iType->setAbsorbPercentElements(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentmagic") {
				iType->setAbsorbPercentMagic(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentenergy") {
				iType->setAbsorbPercentEnergy(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentfire") {
				iType->setAbsorbPercentFire(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentpoison" || lowerCasedKey == "absorbpercentearth") {
				iType->setAbsorbPercentEarth(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentice") {
				iType->setAbsorbPercentIce(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentholy") {
				iType->setAbsorbPercentHoly(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentdeath") {
				iType->setAbsorbPercentDeath(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentlifedrain") {
				iType->setAbsorbPercentLifeDrain(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentmanadrain") {
				iType->setAbsorbPercentManaDrain(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentdrown") {
				iType->setAbsorbPercentDrown(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentphysical") {
				iType->setAbsorbPercentPhysical(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercenthealing") {
				iType->setAbsorbPercentHealing(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "absorbpercentundefined") {
				iType->setAbsorbPercentUndefined(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentall" || lowerCasedKey == "reflectpercentallelements") {
				iType->setReflectPercent(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentelements") {
				iType->setReflectPercentElements(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentmagic") {
				iType->setReflectPercentMagic(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentenergy") {
				iType->setReflectPercentEnergy(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentfire") {
				iType->setReflectPercentFire(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentpoison" || lowerCasedKey == "reflectpercentearth") {
				iType->setReflectPercentEarth(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentice") {
				iType->setReflectPercentIce(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentholy") {
				iType->setReflectPercentHoly(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentdeath") {
				iType->setReflectPercentDeath(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentlifedrain") {
				iType->setReflectPercentLifeDrain(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentmanadrain") {
				iType->setReflectPercentManaDrain(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentdrown") {
				iType->setReflectPercentDrown(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercentphysical") {
				iType->setReflectPercentPhysical(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectpercenthealing") {
				iType->setReflectPercentHealing(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchanceall" || lowerCasedKey == "reflectchanceallelements") {
				iType->setReflectChance(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchanceelements") {
				iType->setReflectChanceElements(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchancemagic") {
				iType->setReflectChanceMagic(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchanceenergy") {
				iType->setReflectChanceEnergy(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchancefire") {
				iType->setReflectChanceFire(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchancepoison" || lowerCasedKey == "reflectchanceearth") {
				iType->setReflectChanceEarth(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchanceice") {
				iType->setReflectChanceIce(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchanceholy") {
				iType->setReflectChanceHoly(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchancedeath") {
				iType->setReflectChanceDeath(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchancelifedrain") {
				iType->setReflectChanceLifeDrain(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchancemanadrain") {
				iType->setReflectChanceManaDrain(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchancedrown") {
				iType->setReflectChanceDrown(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchancephysical") {
				iType->setReflectChancePhysical(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "reflectchancehealing") {
				iType->setReflectChanceHealing(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentall" || lowerCasedKey == "boostpercentallelements") {
				iType->setBoostPercent(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentelements") {
				iType->setBoostPercentElements(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentmagic") {
				iType->setBoostPercentMagic(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentenergy") {
				iType->setBoostPercentEnergy(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentfire") {
				iType->setBoostPercentFire(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentpoison" || lowerCasedKey == "boostpercentearth") {
				iType->setBoostPercentEarth(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentice") {
				iType->setBoostPercentIce(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentholy") {
				iType->setBoostPercentHoly(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentdeath") {
				iType->setBoostPercentDeath(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentlifedrain") {
				iType->setBoostPercentLifeDrain(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentmanadrain") {
				iType->setBoostPercentManaDrain(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentdrown") {
				iType->setBoostPercentDrown(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercentphysical") {
				iType->setBoostPercentPhysical(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "boostpercenthealing") {
				iType->setBoostPercentHealing(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magiclevelenergy") {
				iType->setMagicLevelEnergy(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magiclevelfire") {
				iType->setMagicLevelFire(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magiclevelpoison" || lowerCasedKey == "magiclevelearth") {
				iType->setMagicLevelEarth(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magiclevelice") {
				iType->setMagicLevelIce(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magiclevelholy") {
				iType->setMagicLevelHoly(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magicleveldeath") {
				iType->setMagicLevelDeath(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magiclevellifedrain") {
				iType->setMagicLevelLifeDrain(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magiclevelmanadrain") {
				iType->setMagicLevelManaDrain(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magicleveldrown") {
				iType->setMagicLevelDrown(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magiclevelphysical") {
				iType->setMagicLevelPhysical(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magiclevelhealing") {
				iType->setMagicLevelHealing(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "magiclevelundefined") {
				iType->setMagicLevelUndefined(pugi::cast<int16_t>(value.c_str()));
			} else if (lowerCasedKey == "suppressdrunk") {
				if (pugi::cast<bool>(value.c_str())) {
					iType->setSuppressDrunk();
				}
			} else if (lowerCasedKey == "suppressenergy") {
				if (pugi::cast<bool>(value.c_str())) {
					iType->setSuppressEnergy();
				}
			} else if (lowerCasedKey == "suppressfire") {
				if (pugi::cast<bool>(value.c_str())) {
					iType->setSuppressFire();
				}
			} else if (lowerCasedKey == "suppresspoison") {
				if (pugi::cast<bool>(value.c_str())) {
					iType->setSuppressPoison();
				}
			} else if (lowerCasedKey == "suppressdrown") {
				if (pugi::cast<bool>(value.c_str())) {
					iType->setSuppressDrown();
				}
			} else if (lowerCasedKey == "suppressphysical") {
				if (pugi::cast<bool>(value.c_str())) {
					iType->setSuppressBleeding();
				}
			} else if (lowerCasedKey == "suppressfreeze") {
				if (pugi::cast<bool>(value.c_str())) {
					iType->setSuppressFreezing();
				}
			} else if (lowerCasedKey == "suppressdazzle") {
				if (pugi::cast<bool>(value.c_str())) {
					iType->setSuppressDazzle();
				}
			} else if (lowerCasedKey == "suppresscurse") {
				if (pugi::cast<bool>(value.c_str())) {
					iType->setSuppressCurse();
				}
			} else if (lowerCasedKey == "field") {
				auto cd = (new ConditionDamageBuilder())->loadFromXMLNode(attributeNode, false);
				iType->setField(value, cd);
			} else if (lowerCasedKey == "replaceable") {
				iType->replaceable = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "partnerdirection") {
				iType->bedPartnerDir = getDirection(value);
			} else if (lowerCasedKey == "leveldoor") {
				iType->levelDoor = pugi::cast<uint32_t>(value.c_str());
			} else if (lowerCasedKey == "maletransformto" || lowerCasedKey == "malesleeper") {
				iType->setMaleTransformTo(pugi::cast<uint16_t>(value.c_str()));
			} else if (lowerCasedKey == "femaletransformto" || lowerCasedKey == "femalesleeper") {
				iType->setFemaleTransformTo(pugi::cast<uint16_t>(value.c_str()));
			} else if (lowerCasedKey == "transformto") {
				iType->transformToFree = pugi::cast<uint16_t>(value.c_str());
			} else if (lowerCasedKey == "destroyto") {
				iType->destroyTo = pugi::cast<uint16_t>(value.c_str());
			} else if (lowerCasedKey == "elementice") {
				iType->setIceElementDamage(pugi::cast<uint16_t>(value.c_str()));
			} else if (lowerCasedKey == "elementearth") {
				iType->setEarthElementDamage(pugi::cast<uint16_t>(value.c_str()));
			} else if (lowerCasedKey == "elementfire") {
				iType->setFireElementDamage(pugi::cast<uint16_t>(value.c_str()));
			} else if (lowerCasedKey == "elementenergy") {
				iType->setEnergyElementDamage(pugi::cast<uint16_t>(value.c_str()));
			} else if (lowerCasedKey == "elementdeath") {
				iType->setDeathElementDamage(pugi::cast<uint16_t>(value.c_str()));
			} else if (lowerCasedKey == "elementholy") {
				iType->setHolyElementDamage(pugi::cast<uint16_t>(value.c_str()));
			} else if (lowerCasedKey == "walkstack") {
				iType->walkStack = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "blocking") {
				iType->blockSolid = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "allowdistread") {
				iType->allowDistRead = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "storeitem") {
				iType->storeItem = pugi::cast<bool>(value.c_str());
			} else if (lowerCasedKey == "worth") {
				iType->setWorth(pugi::cast<uint64_t>(value.c_str()));
			} else if (lowerCasedKey == "supply") {
				iType->supply = pugi::cast<bool>(value.c_str());
			}

			return Items::getInstance().getItemType(fromId);
		}
	}
}

void ItemType::setField(const std::string& element, ConditionDamage* cd)
{
	this->group = ITEM_GROUP_MAGICFIELD;
	this->type = ITEM_TYPE_MAGICFIELD;

	if (element == "fire") {
		this->combatType = COMBAT_FIREDAMAGE;
	} else if (element == "energy") {
		this->combatType = COMBAT_ENERGYDAMAGE;
	} else if (element == "poison") {
		this->combatType = COMBAT_EARTHDAMAGE;
	} else if (element == "drown") {
		this->combatType = COMBAT_DROWNDAMAGE;
	} else if (element == "physical") {
		this->combatType = COMBAT_PHYSICALDAMAGE;
	} else {
		this->combatType = COMBAT_NONE;
		std::cout << "[Warning - ItemType::setField] Unknown field value: " << element << std::endl;
	}

	this->conditionDamage.reset(cd);
}

void ItemType::setMaleTransformTo(uint16_t id)
{
	this->transformToOnUse[PLAYERSEX_MALE] = id;
	ItemType* other = Items::getInstance().getItemType(id);
	if (other->transformToFree == 0) {
		other->transformToFree = this->id;
	}

	if (this->transformToOnUse[PLAYERSEX_FEMALE] == 0) {
		this->transformToOnUse[PLAYERSEX_FEMALE] = id;
	}
}

void ItemType::setFemaleTransformTo(uint16_t id)
{
	this->transformToOnUse[PLAYERSEX_FEMALE] = id;
	ItemType* other = Items::getInstance().getItemType(id);
	if (other->transformToFree == 0) {
		other->transformToFree = this->id;
	}

	if (this->transformToOnUse[PLAYERSEX_MALE] == 0) {
		this->transformToOnUse[PLAYERSEX_MALE] = id;
	}
}

ConditionDamage* ConditionDamageBuilder::loadFromXMLNode(pugi::xml_node node, bool)
{
	CombatType_t combatType = COMBAT_NONE;

	auto element = node.attribute("value").as_string();
	if (element == "fire") {
		this->conditionDamage = std::make_unique<ConditionDamage>(CONDITIONID_COMBAT, CONDITION_FIRE);
	} else if (element == "energy") {
		this->conditionDamage = std::make_unique<ConditionDamage>(CONDITIONID_COMBAT, CONDITION_ENERGY);
	} else if (element == "poison") {
		this->conditionDamage = std::make_unique<ConditionDamage>(CONDITIONID_COMBAT, CONDITION_POISON);
	} else if (element == "drown") {
		this->conditionDamage = std::make_unique<ConditionDamage>(CONDITIONID_COMBAT, CONDITION_DROWN);
	} else if (element == "physical") {
		this->conditionDamage = std::make_unique<ConditionDamage>(CONDITIONID_COMBAT, CONDITION_BLEEDING);
	} else {
		std::cout << "[Warning - ConditionDamageBuilder::loadFromXMLNode] Unknown field value: " << element
		          << std::endl;
		return nullptr;
	}

	int32_t initDamage = -1;
	int32_t damage = 0;
	for (auto child : node.children()) {
		pugi::xml_attribute keyAttribute = child.attribute("key");
		if (!keyAttribute) {
			continue;
		}

		pugi::xml_attribute valueAttribute = child.attribute("value");
		if (!valueAttribute) {
			continue;
		}

		auto key = boost::algorithm::to_lower_copy<std::string>(keyAttribute.as_string());
		auto value = boost::algorithm::to_lower_copy<std::string>(valueAttribute.as_string());
		if (key == "initdamage") {
			initDamage = pugi::cast<int32_t>(value.c_str());
		} else if (key == "ticks") {
			this->ticks = pugi::cast<uint32_t>(value.c_str());
		} else if (key == "count") {
			this->count = pugi::cast<int32_t>(value.c_str());
		} else if (key == "start") {
			this->start = pugi::cast<int32_t>(value.c_str());
		} else if (key == "damage") {
			damage = -pugi::cast<int32_t>(value.c_str());
		}

		this->withDamage(damage, initDamage);

		this->conditionDamage->setParam(CONDITION_PARAM_FIELD, 1);

		if (this->conditionDamage->getTotalDamage() > 0) {
			this->conditionDamage->setParam(CONDITION_PARAM_FORCEUPDATE, 1);
		}

		return this->conditionDamage.release();
	}
}
