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
	uint32_t referenceCounter = 0;
	bool loadedFromMap = false;

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

	// TODO: Move to Container.h
	static Container* CreateItemAsContainer(const uint16_t type, uint16_t size);

	void incrementReferenceCounter() { ++referenceCounter; }
	void decrementReferenceCounter()
	{
		if (--referenceCounter == 0) {
			delete this;
		}
	}

	uint16_t getItemCount() const { return count; }
	void setItemCount(uint8_t n) { count = n; }

	uint16_t getCharges() const { return static_cast<uint16_t>(iAttributes->getIntAttr(ITEM_ATTRIBUTE_CHARGES)); }
	void setCharges(uint16_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_CHARGES, n); }

	uint16_t getFluidType() const { return static_cast<uint16_t>(iAttributes->getIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE)); }
	void setFluidType(uint16_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE, n); }

	uint32_t getDuration() const
	{
		if (!iAttributes) {
			return 0;
		}
		return iAttributes->getIntAttr(ITEM_ATTRIBUTE_DURATION);
	}
	void setDuration(int32_t time) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_DURATION, time); }
	uint32_t getDefaultDuration() const { return Items::getInstance().getItemType(id)->decayTime * 1000; }
	void setDefaultDuration()
	{
		uint32_t duration = getDefaultDuration();
		if (duration != 0) {
			setDuration(duration);
		}
	}

	void setDecaying(ItemDecayState decayState) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_DECAYSTATE, decayState); }
};

// #endif
