#ifndef PR_ITEMS_H
#define PR_ITEMS_H

#include "libs/item/ClientIdToServerIdMap.h"
#include "libs/item/ItemType.h"
#include "libs/util/otb/OTBLoadable.h"
#include "libs/util/xml/XMLLoadable.h"

constexpr auto OTBI = OTBIdentifier{{'O', 'T', 'B', 'I'}};

class Items final : virtual public XMLLoadable, OTBLoadable
{
private:
	Items() : OTBLoadable("data/items/items.otb", OTBI)
	{
		this->filepath = "data/items/items.xml";
		this->childNode = "items";
	}

	bool load(pugi::xml_node node, bool) override;
	bool load(const OTBNode& node, PropStream stream) override;

	std::vector<ItemType> items;
	ClientIdToServerIdMap clientIdToServerIdMap;

	void addItem(ItemType& item)
	{
		if (item.id >= items.size()) {
			items.resize(item.id + 1);
		}

		this->items.insert(this->items.begin() + item.id, std::move(item));
	}

public:
	Items(const Items&) = delete;
	Items& operator=(const Items&) = delete;

	bool loadFromOTB() override;
};

#endif
