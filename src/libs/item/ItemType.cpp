#include "libs/item/ItemType.h"

#include "libs/item/Items.h"
#include "libs/item/itemmaps.h"
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
			} else if (lowerCasedKey == "shoottype") {
			} else if (lowerCasedKey == "effect") {
			} else if (lowerCasedKey == "range") {
			} else if (lowerCasedKey == "stopduration") {
			} else if (lowerCasedKey == "decayto") {
			} else if (lowerCasedKey == "transformequipto") {
			} else if (lowerCasedKey == "transformdeequipto") {
			} else if (lowerCasedKey == "duration") {
			} else if (lowerCasedKey == "showduration") {
			} else if (lowerCasedKey == "charges") {
			} else if (lowerCasedKey == "showcharges") {
			} else if (lowerCasedKey == "showattributes") {
			} else if (lowerCasedKey == "hitchance") {
			} else if (lowerCasedKey == "maxhitchance") {
			} else if (lowerCasedKey == "invisible") {
			} else if (lowerCasedKey == "speed") {
			} else if (lowerCasedKey == "healthgain") {
			} else if (lowerCasedKey == "healthticks") {
			} else if (lowerCasedKey == "managain") {
			} else if (lowerCasedKey == "manaticks") {
			} else if (lowerCasedKey == "manashield") {
			} else if (lowerCasedKey == "skillsword") {
			} else if (lowerCasedKey == "skillaxe") {
			} else if (lowerCasedKey == "skillclub") {
			} else if (lowerCasedKey == "skilldist") {
			} else if (lowerCasedKey == "skillfish") {
			} else if (lowerCasedKey == "skillshield") {
			} else if (lowerCasedKey == "skillfist") {
			} else if (lowerCasedKey == "maxhitpoints") {
			} else if (lowerCasedKey == "maxhitpointspercent") {
			} else if (lowerCasedKey == "maxmanapoints") {
			} else if (lowerCasedKey == "maxmanapointspercent") {
			} else if (lowerCasedKey == "magicpoints") {
			} else if (lowerCasedKey == "magiclevelpoints") {
			} else if (lowerCasedKey == "magicpointspercent") {
			} else if (lowerCasedKey == "criticalhitchance") {
			} else if (lowerCasedKey == "criticalhitamount") {
			} else if (lowerCasedKey == "lifeleechchance") {
			} else if (lowerCasedKey == "lifeleechamount") {
			} else if (lowerCasedKey == "manaleechchance") {
			} else if (lowerCasedKey == "manaleechamount") {
			} else if (lowerCasedKey == "fieldabsorbpercentenergy") {
			} else if (lowerCasedKey == "fieldabsorbpercentfire") {
			} else if (lowerCasedKey == "fieldabsorbpercentpoison") {
			} else if (lowerCasedKey == "fieldabsorbpercentearth") {
			} else if (lowerCasedKey == "absorbpercentall") {
			} else if (lowerCasedKey == "absorbpercentallelements") {
			} else if (lowerCasedKey == "absorbpercentelements") {
			} else if (lowerCasedKey == "absorbpercentmagic") {
			} else if (lowerCasedKey == "absorbpercentenergy") {
			} else if (lowerCasedKey == "absorbpercentfire") {
			} else if (lowerCasedKey == "absorbpercentpoison") {
			} else if (lowerCasedKey == "absorbpercentearth") {
			} else if (lowerCasedKey == "absorbpercentice") {
			} else if (lowerCasedKey == "absorbpercentholy") {
			} else if (lowerCasedKey == "absorbpercentdeath") {
			} else if (lowerCasedKey == "absorbpercentlifedrain") {
			} else if (lowerCasedKey == "absorbpercentmanadrain") {
			} else if (lowerCasedKey == "absorbpercentdrown") {
			} else if (lowerCasedKey == "absorbpercentphysical") {
			} else if (lowerCasedKey == "absorbpercenthealing") {
			} else if (lowerCasedKey == "absorbpercentundefined") {
			} else if (lowerCasedKey == "reflectpercentall") {
			} else if (lowerCasedKey == "reflectpercentallelements") {
			} else if (lowerCasedKey == "reflectpercentelements") {
			} else if (lowerCasedKey == "reflectpercentmagic") {
			} else if (lowerCasedKey == "reflectpercentenergy") {
			} else if (lowerCasedKey == "reflectpercentfire") {
			} else if (lowerCasedKey == "reflectpercentpoison") {
			} else if (lowerCasedKey == "reflectpercentearth") {
			} else if (lowerCasedKey == "reflectpercentice") {
			} else if (lowerCasedKey == "reflectpercentholy") {
			} else if (lowerCasedKey == "reflectpercentdeath") {
			} else if (lowerCasedKey == "reflectpercentlifedrain") {
			} else if (lowerCasedKey == "reflectpercentmanadrain") {
			} else if (lowerCasedKey == "reflectpercentdrown") {
			} else if (lowerCasedKey == "reflectpercentphysical") {
			} else if (lowerCasedKey == "reflectpercenthealing") {
			} else if (lowerCasedKey == "reflectchanceall") {
			} else if (lowerCasedKey == "reflectchanceallelements") {
			} else if (lowerCasedKey == "reflectchanceelements") {
			} else if (lowerCasedKey == "reflectchancemagic") {
			} else if (lowerCasedKey == "reflectchanceenergy") {
			} else if (lowerCasedKey == "reflectchancefire") {
			} else if (lowerCasedKey == "reflectchancepoison") {
			} else if (lowerCasedKey == "reflectchanceearth") {
			} else if (lowerCasedKey == "reflectchanceice") {
			} else if (lowerCasedKey == "reflectchanceholy") {
			} else if (lowerCasedKey == "reflectchancedeath") {
			} else if (lowerCasedKey == "reflectchancelifedrain") {
			} else if (lowerCasedKey == "reflectchancemanadrain") {
			} else if (lowerCasedKey == "reflectchancedrown") {
			} else if (lowerCasedKey == "reflectchancephysical") {
			} else if (lowerCasedKey == "reflectchancehealing") {
			} else if (lowerCasedKey == "boostpercentall") {
			} else if (lowerCasedKey == "boostpercentallelements") {
			} else if (lowerCasedKey == "boostpercentelements") {
			} else if (lowerCasedKey == "boostpercentmagic") {
			} else if (lowerCasedKey == "boostpercentenergy") {
			} else if (lowerCasedKey == "boostpercentfire") {
			} else if (lowerCasedKey == "boostpercentpoison") {
			} else if (lowerCasedKey == "boostpercentearth") {
			} else if (lowerCasedKey == "boostpercentice") {
			} else if (lowerCasedKey == "boostpercentholy") {
			} else if (lowerCasedKey == "boostpercentdeath") {
			} else if (lowerCasedKey == "boostpercentlifedrain") {
			} else if (lowerCasedKey == "boostpercentmanadrain") {
			} else if (lowerCasedKey == "boostpercentdrown") {
			} else if (lowerCasedKey == "boostpercentphysical") {
			} else if (lowerCasedKey == "boostpercenthealing") {
			} else if (lowerCasedKey == "magiclevelenergy") {
			} else if (lowerCasedKey == "magiclevelfire") {
			} else if (lowerCasedKey == "magiclevelpoison") {
			} else if (lowerCasedKey == "magiclevelearth") {
			} else if (lowerCasedKey == "magiclevelice") {
			} else if (lowerCasedKey == "magiclevelholy") {
			} else if (lowerCasedKey == "magicleveldeath") {
			} else if (lowerCasedKey == "magiclevellifedrain") {
			} else if (lowerCasedKey == "magiclevelmanadrain") {
			} else if (lowerCasedKey == "magicleveldrown") {
			} else if (lowerCasedKey == "magiclevelphysical") {
			} else if (lowerCasedKey == "magiclevelhealing") {
			} else if (lowerCasedKey == "magiclevelundefined") {
			} else if (lowerCasedKey == "suppressdrunk") {
			} else if (lowerCasedKey == "suppressenergy") {
			} else if (lowerCasedKey == "suppressfire") {
			} else if (lowerCasedKey == "suppresspoison") {
			} else if (lowerCasedKey == "suppressdrown") {
			} else if (lowerCasedKey == "suppressphysical") {
			} else if (lowerCasedKey == "suppressfreeze") {
			} else if (lowerCasedKey == "suppressdazzle") {
			} else if (lowerCasedKey == "suppresscurse") {
			} else if (lowerCasedKey == "field") {
			} else if (lowerCasedKey == "replaceable") {
			} else if (lowerCasedKey == "partnerdirection") {
			} else if (lowerCasedKey == "leveldoor") {
			} else if (lowerCasedKey == "maletransformto") {
			} else if (lowerCasedKey == "malesleeper") {
			} else if (lowerCasedKey == "femaletransformto") {
			} else if (lowerCasedKey == "femalesleeper") {
			} else if (lowerCasedKey == "transformto") {
			} else if (lowerCasedKey == "destroyto") {
			} else if (lowerCasedKey == "elementice") {
			} else if (lowerCasedKey == "elementearth") {
			} else if (lowerCasedKey == "elementfire") {
			} else if (lowerCasedKey == "elementenergy") {
			} else if (lowerCasedKey == "elementdeath") {
			} else if (lowerCasedKey == "elementholy") {
			} else if (lowerCasedKey == "walkstack") {
			} else if (lowerCasedKey == "blocking") {
			} else if (lowerCasedKey == "allowdistread") {
			} else if (lowerCasedKey == "storeitem") {
			} else if (lowerCasedKey == "worth") {
			} else if (lowerCasedKey == "supply") {
			}

			return Items::getInstance().getItemType(fromId);
		}
	}
}
