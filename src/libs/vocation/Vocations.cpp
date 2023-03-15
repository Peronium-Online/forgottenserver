#include "libs/vocation/Vocations.h"

#include "libs/util/tools/pugicast.h"
#include "libs/util/tools/strings.h"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

bool Vocations::load(pugi::xml_node node, bool)
{
	pugi::xml_attribute attr = node.attribute("id");
	if (!attr) {
		std::cout << "[Warning - Vocations::loadFromXml] Missing vocation id" << std::endl;
		return false;
	}

	uint16_t id = pugi::cast<uint16_t>(attr.value());
	auto res = vocationsMap.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(id));
	Vocation& voc = res.first->second;

	node.remove_attribute("id");
	for (auto attrNode : node.attributes()) {
		const char* attrName = attrNode.name();
		if (caseInsensitiveEqual(attrName, "name")) {
			voc.name = attrNode.as_string();
		} else if (caseInsensitiveEqual(attrName, "allowpvp")) {
			voc.allowPvp = attrNode.as_bool();
		} else if (caseInsensitiveEqual(attrName, "clientid")) {
			voc.clientId = pugi::cast<uint16_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "description")) {
			voc.description = attrNode.as_string();
		} else if (caseInsensitiveEqual(attrName, "gaincap")) {
			voc.gainCap = pugi::cast<uint32_t>(attrNode.value()) * 100;
		} else if (caseInsensitiveEqual(attrName, "gainhp")) {
			voc.gainHP = pugi::cast<uint32_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "gainmana")) {
			voc.gainMana = pugi::cast<uint32_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "gainhpticks")) {
			voc.gainHealthTicks = pugi::cast<uint32_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "gainhpamount")) {
			voc.gainHealthAmount = pugi::cast<uint32_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "gainmanaticks")) {
			voc.gainManaTicks = pugi::cast<uint32_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "gainmanaamount")) {
			voc.gainManaAmount = pugi::cast<uint32_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "manamultiplier")) {
			voc.manaMultiplier = pugi::cast<float>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "attackspeed")) {
			voc.attackSpeed = pugi::cast<uint32_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "basespeed")) {
			voc.baseSpeed = pugi::cast<uint32_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "soulmax")) {
			voc.soulMax = pugi::cast<uint16_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "gainsoulticks")) {
			voc.gainSoulTicks = pugi::cast<uint16_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "fromvoc")) {
			voc.fromVocation = pugi::cast<uint32_t>(attrNode.value());
		} else if (caseInsensitiveEqual(attrName, "nopongkicktime")) {
			voc.noPongKickTime = pugi::cast<uint32_t>(attrNode.value()) * 1000;
		} else {
			std::cout << "[Notice - Vocations::loadFromXml] Unknown attribute: \"" << attrName
			          << "\" for vocation: " << voc.id << std::endl;
		}
	}

	for (auto childNode : node.children()) {
		if (caseInsensitiveEqual(childNode.name(), "skill")) {
			if ((attr = childNode.attribute("id"))) {
				uint16_t skillId = pugi::cast<uint16_t>(attr.value());
				if (skillId <= SKILL_LAST) {
					voc.skillMultipliers[skillId] = pugi::cast<double>(childNode.attribute("multiplier").value());
				} else {
					std::cout << "[Notice - Vocations::loadFromXml] No valid skill id: " << skillId
					          << " for vocation: " << voc.id << std::endl;
				}
			} else {
				std::cout << "[Notice - Vocations::loadFromXml] Missing skill id for vocation: " << voc.id << std::endl;
			}
		} else if (caseInsensitiveEqual(childNode.name(), "formula")) {
			if ((attr = childNode.attribute("meleeDamage"))) {
				voc.meleeDamageMultiplier = pugi::cast<float>(attr.value());
			}

			if ((attr = childNode.attribute("distDamage"))) {
				voc.distDamageMultiplier = pugi::cast<float>(attr.value());
			}

			if ((attr = childNode.attribute("defense"))) {
				voc.defenseMultiplier = pugi::cast<float>(attr.value());
			}

			if ((attr = childNode.attribute("armor"))) {
				voc.armorMultiplier = pugi::cast<float>(attr.value());
			}
		}
	}

	return true;
};

Vocation* Vocations::getVocation(uint16_t id)
{
	auto it = vocationsMap.find(id);
	if (it == vocationsMap.end()) {
		std::cout << "[Warning - Vocations::getVocation] Vocation " << id << " not found." << std::endl;
		return nullptr;
	}
	return &it->second;
}

int32_t Vocations::getVocationId(const std::string& name) const
{
	auto it = std::find_if(vocationsMap.begin(), vocationsMap.end(), [&name](auto it) {
		return name.size() == it.second.name.size() &&
		       std::equal(name.begin(), name.end(), it.second.name.begin(),
		                  [](char a, char b) { return std::tolower(a) == std::tolower(b); });
	});
	return it != vocationsMap.end() ? it->first : -1;
}

uint16_t Vocations::getPromotedVocation(uint16_t id) const
{
	auto it = std::find_if(vocationsMap.begin(), vocationsMap.end(),
	                       [id](auto it) { return it.second.fromVocation == id && it.first != id; });
	return it != vocationsMap.end() ? it->first : VOCATION_NONE;
}
