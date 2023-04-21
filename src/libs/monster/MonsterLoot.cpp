#include "libs/monster/MonsterLoot.h"

#include "../../item.h"
#include "../../items.h"

#include <boost/algorithm/string.hpp>
#include <iostream>

MonsterLoot* MonsterLoot::setItemId(uint16_t itemId)
{
	const ItemType& it = Item::items.getItemType(itemId);
	if (it.name.empty()) {
		std::cout << "[Warning - MonsterLoot::setItemId] Unknown loot item id \"" << itemId << "\". " << std::endl;
		throw std::invalid_argument("Invalid item ID provided");
	}

	this->itemId = itemId;
	uint32_t charges = Item::items[itemId].charges;
	if (charges != 0) {
		this->subType = charges;
	}

	return this;
}

MonsterLoot* MonsterLoot::setItemIdByName(std::string name)
{
	auto ids = Item::items.nameToItems.equal_range(boost::algorithm::to_lower_copy<std::string>(name));

	// found anything?
	if (ids.first == Item::items.nameToItems.cend()) {
		std::cout << "[Warning - MonsterLoot::setItemIdByName] Unknown loot item \"" << name << "\". " << std::endl;
		throw std::invalid_argument("Invalid item name provided");
	}

	// found more than one?
	if (std::next(ids.first) != ids.second) {
		std::cout << "[Warning - MonsterLoot::setItemIdByName] Non-unique loot item \"" << name << "\". " << std::endl;
		throw std::invalid_argument("Invalid item name provided");
	}

	uint32_t id = ids.first->second;
	this->itemId = id;

	uint32_t charges = Item::items[id].charges;
	if (charges != 0) {
		this->subType = charges;
	}

	return this;
}

MonsterLoot* MonsterLoot::setCountMax(uint32_t countMax)
{
	this->countMax = countMax;
	return this;
}

MonsterLoot* MonsterLoot::setChance(uint32_t chance)
{
	if (chance > static_cast<int32_t>(MAX_LOOTCHANCE)) {
		std::cout << "[Warning - MonsterLoot::setChance] Invalid \"chance\" " << chance << " used for loot, the max is "
		          << MAX_LOOTCHANCE << ". " << std::endl;
	}
	this->chance = chance;
	return this;
}

MonsterLoot* MonsterLoot::setSubType(int32_t subType)
{
	this->subType = subType;
	return this;
}

MonsterLoot* MonsterLoot::setActionId(int32_t actionId)
{
	this->actionId = actionId;
	return this;
}

MonsterLoot* MonsterLoot::setText(std::string text)
{
	this->text = text;
	return this;
}

MonsterLoot* MonsterLoot::addChildLoot(MonsterLoot childLoot)
{
	this->childrenLoots.emplace_back(std::move(childLoot));
	return this;
}

MonsterLoot* MonsterLoot::loadFromXMLNode(pugi::xml_node node, bool reloading)
{
	try {
		pugi::xml_attribute attr;
		if (attr = node.attribute("id")) {
			int32_t id = pugi::cast<int32_t>(attr.value());

			this->setItemId(id);
		} else if (attr = node.attribute("name")) {
			auto name = attr.as_string();

			this->setItemIdByName(name);
		} else {
			std::cout << "[Error - loadFromXMLNode]: Missing required attribute \"id\" or \"name\"" << std::endl;
			return nullptr;
		}

		if (attr = node.attribute("countmax")) {
			int32_t countMax = pugi::cast<int32_t>(attr.value());
			this->setCountMax(countMax);
		} else {
			this->setCountMax(1);
		}

		if ((attr = node.attribute("chance")) || (attr = node.attribute("chance1"))) {
			int32_t chance = pugi::cast<int32_t>(attr.value());
			this->setChance(chance);
		} else {
			this->setChance(MAX_LOOTCHANCE);
		}

		if (attr = node.attribute("subtype")) {
			int32_t subType = pugi::cast<int32_t>(attr.value());
			this->setSubType(subType);
		}

		if (attr = node.attribute("actionId")) {
			int32_t actionId = pugi::cast<int32_t>(attr.value());
			this->setActionId(actionId);
		}

		if (attr = node.attribute("text")) {
			std::string text = attr.as_string();
			this->setText(text);
		}

		if (Item::items[this->itemId].isContainer()) {
			// NOTE: <inside> attribute was left for backwards compatibility with pre 1.x TFS versions. Please don't use
			// it, if you don't have to.
			for (auto subNode : node.child("inside") ? node.child("inside").children() : node.children()) {
				MonsterLoot childLoot;
				if (childLoot.loadFromXMLNode(subNode, reloading)) {
					this->addChildLoot(std::move(childLoot));
				}
			}
		}

		return this;
	} catch (std::exception& e) {
		std::cerr << "[Error - loadFromXMLNode]: " << e.what() << std::endl;
		return nullptr;
	}
}
