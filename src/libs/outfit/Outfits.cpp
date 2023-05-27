#include "libs/outfit/Outfits.h"

#include "libs/util/tools/pugicast.h"

const Outfit* Outfits::getOutfitByLookType(PlayerSex_t sex, uint16_t lookType) const
{
	for (const Outfit& outfit : outfits[sex]) {
		if (outfit.lookType == lookType) {
			return &outfit;
		}
	}
	return nullptr;
}

const Outfit* Outfits::getOutfitByLookType(uint16_t lookType) const
{
	for (uint8_t sex = PLAYERSEX_FEMALE; sex <= PLAYERSEX_LAST; sex++) {
		for (const Outfit& outfit : outfits[sex]) {
			if (outfit.lookType == lookType) {
				return &outfit;
			}
		}
	}
	return nullptr;
}

bool Outfits::load(pugi::xml_node node, bool)
{
	const std::string& name = node.attribute("name").as_string();
	if (name.empty()) {
		std::cout << "[Warning - Outfits::load] Missing outfit name." << std::endl;
		return false;
	}

	pugi::xml_attribute attr;
	if ((attr = node.attribute("enabled")) && !attr.as_bool()) {
		std::cout << "[Warning - Outfits::load] Outfit " << name << " is disabled." << std::endl;
		return false;
	}

	if (!(attr = node.attribute("type"))) {
		std::cout << "[Warning - Outfits::load] Missing outfit type for outfit " << name << "." << std::endl;
		return false;
	}

	uint16_t type = pugi::cast<uint16_t>(attr.value());
	if (type > PLAYERSEX_LAST) {
		std::cout << "[Warning - Outfits::load] Invalid outfit type " << type << " for outfit " << name << "."
		          << std::endl;
		return false;
	}

	pugi::xml_attribute lookTypeAttribute = node.attribute("looktype");
	if (!lookTypeAttribute) {
		std::cout << "[Warning - Outfits::load] Missing looktype for outfit " << name << "." << std::endl;
		return false;
	}

	auto lookType = pugi::cast<uint16_t>(lookTypeAttribute.value());
	auto premium = node.attribute("premium").as_bool();
	auto unlocked = node.attribute("unlocked").as_bool(true);

	outfits[type].emplace_back(name, lookType, premium, unlocked);

	return true;
}
