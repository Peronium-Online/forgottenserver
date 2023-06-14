// #ifdef PR_ITEM_H
// #define PR_ITEM_H

#include "../../../src/thing.h"
#include "libs/item/ItemAttributes.h"
#include "libs/item/itemenums.h"
#include "libs/util/otb/OTBLoadable.h"

using ItemList = std::list<Item*>;
using ItemDeque = std::deque<Item*>;

class Item : virtual public Thing
{
private:
	std::unique_ptr<ItemAttributes> iAttributes;

	uint16_t count = 1;

protected:
	uint16_t id; // the same id as in ItemType

public:
	Item(const uint16_t type, uint16_t count = 0);
	Item(const Item& i);
	virtual ~Item() = default;

	Item& operator=(const Item&) = delete;
	bool equals(const Item* otherItem) const;
	virtual Item* clone() const;

	// Factory member to create item of right type based on type
	static Item* CreateItem(const uint16_t type, uint16_t count = 0);
	static Container* CreateItemAsContainer(const uint16_t type, uint16_t size);
	static Item* CreateItem(PropStream& propStream);

	uint16_t getItemCount() const { return count; }
	void setItemCount(uint8_t n) { count = n; }

	uint16_t getCharges() const { return static_cast<uint16_t>(iAttributes->getIntAttr(ITEM_ATTRIBUTE_CHARGES)); }
	void setCharges(uint16_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_CHARGES, n); }

	uint16_t getFluidType() const { return static_cast<uint16_t>(iAttributes->getIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE)); }
	void setFluidType(uint16_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE, n); }

	void setDuration(int32_t time) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_DURATION, time); }
	uint32_t getDefaultDuration() const { return Items::getInstance().getItemType(id)->decayTime * 1000; }
	void setDefaultDuration()
	{
		uint32_t duration = getDefaultDuration();
		if (duration != 0) {
			setDuration(duration);
		}
	}
};

// #endif
