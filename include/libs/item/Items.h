#ifndef PR_ITEMS_H
#define PR_ITEMS_H

#include "libs/item/ClientIdToServerIdMap.h"
#include "libs/item/ItemType.h"
#include "libs/util/otb/OTBLoadable.h"
#include "libs/util/xml/XMLLoadable.h"

#include <map>

using NameMap = std::unordered_map<std::string, uint16_t>;
using CurrencyMap = std::map<uint64_t, uint16_t, std::greater<uint64_t>>;

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
	NameMap nameToItems;
	CurrencyMap currencyItems;

	void addItem(ItemType& item)
	{
		if (item.id >= items.size()) {
			items.resize(item.id + 1);
		}

		this->items.insert(this->items.begin() + item.id, std::move(item));
	}

	void addNameToItems(const std::string& name, uint16_t id)
	{
		std::string lowerCaseName = boost::algorithm::to_lower_copy(name);
		if (nameToItems.find(lowerCaseName) == nameToItems.end()) {
			nameToItems.emplace(std::move(lowerCaseName), id);
		}
	}

public:
	Items(const Items&) = delete;
	Items& operator=(const Items&) = delete;

	static Items& getInstance()
	{
		static Items instance;
		return instance;
	}

	bool loadFromOTB() override;

	ItemType* getItemType(size_t id)
	{
		if (id < items.size()) {
			return &items[id];
		}
		return &items.front();
	}

	bool hasCurrencyItemOf(uint64_t amount) { return currencyItems.find(amount) != currencyItems.end(); }

	bool isCurrencyItem(uint16_t id)
	{
		for (auto& currencyPair : currencyItems) {
			if (currencyPair.second == id) {
				return true;
			}
		}

		return false;
	}

	void addCurrencyItem(uint64_t amount, uint16_t id) { currencyItems.insert(CurrencyMap::value_type(amount, id)); }
};

#endif
