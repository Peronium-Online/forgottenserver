// #ifdef PR_ITEM_H
// #define PR_ITEM_H

#include "../../../src/thing.h"
#include "libs/item/ItemAttributes.h"
#include "libs/item/ItemType.h"
#include "libs/item/Items.h"
#include "libs/item/itemenums.h"
#include "libs/util/otb/OTBLoadable.h"

using ItemList = std::list<Item*>;
using ItemDeque = std::deque<Item*>;

struct ItemAttrError : std::runtime_error
{
	ItemAttrTypesIndex attr;
	ItemAttrError(ItemAttrTypesIndex attr, const char* msg) : attr(attr), std::runtime_error(msg) {}

	const char* what() const noexcept override
	{
		std::string errorMsg = "ItemAttrError(" + std::to_string(attr) + "): " + std::runtime_error::what();
		return errorMsg.c_str();
	}
};

class Item : virtual public Thing
{
private:
	std::unique_ptr<ItemAttributes> iAttributes;
	uint32_t referenceCounter = 0;
	bool loadedFromMap = false;

	uint16_t count = 1;

protected:
	uint16_t id; // the same id as in ItemType

	const ItemType* iType;

public:
	Item(const uint16_t type, uint16_t count = 0);
	Item(const Item& i);
	virtual ~Item() = default;

	Item& operator=(const Item&) = delete;
	bool equals(const Item* otherItem) const;
	virtual Item* clone() const;

	// TODO: Move to Container.h
	static Container* CreateItemAsContainer(const uint16_t type, uint16_t size);

	virtual void setAttributeFromPropStream(ItemAttrTypesIndex idx, PropStream& stream);

	bool isBlocking() const { return iType->blockSolid; }
	bool isStackable() const { return iType->stackable; }
	bool isAlwaysOnTop() const { return iType->alwaysOnTop; }
	bool isGroundTile() const { return iType->isGroundTile(); }
	bool isMagicField() const { return iType->isMagicField(); }
	bool isMoveable() const { return iType->moveable; }
	bool isPickupable() const { return iType->pickupable; }
	bool isUseable() const { return iType->useable; }
	bool isHangable() const { return iType->isHangable; }
	bool isRotatable() const
	{
		auto it = iType;

		return it->rotatable && it->rotateTo;
	}
	bool isPodium() const { return iType->isPodium(); }
	bool hasWalkStack() const { return iType->walkStack; }
	bool isSupply() const { return iType->isSupply(); }
	bool isPushable() const override final { return isMoveable(); }

	void setSubType(uint16_t n);

	void incrementReferenceCounter() { ++referenceCounter; }
	void decrementReferenceCounter()
	{
		if (--referenceCounter == 0) {
			delete this;
		}
	}

	std::string getDescription(int32_t lookDistance) const override final
	{
		// item descriptions moved to lua
		return "";
	};

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
	void setDuration(int32_t time) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_DURATION, std::max<int32_t>(0, time)); }
	uint32_t getDefaultDuration() const { return iType->decayTime * 1000; }
	void setDefaultDuration()
	{
		uint32_t duration = getDefaultDuration();
		if (duration != 0) {
			setDuration(duration);
		}
	}

	void setDecaying(ItemDecayState decayState) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_DECAYSTATE, decayState); }

	int32_t getThrowRange() const override final { return (isPickupable() ? 15 : 2); }

	virtual uint32_t getWeight() const;
	uint32_t getBaseWeight() const
	{
		if (iAttributes->hasAttribute(ITEM_ATTRIBUTE_WEIGHT)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_WEIGHT);
		}
		return iType->weight;
	}

	void setActionId(uint16_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_ACTIONID, n); }

	void setUniqueId(uint16_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_UNIQUEID, n); }

	void setText(const std::string& text) { iAttributes->setStrAttr(ITEM_ATTRIBUTE_TEXT, text); }

	void setDate(int32_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_DATE, n); }

	void setWriter(const std::string& writer) { iAttributes->setStrAttr(ITEM_ATTRIBUTE_WRITER, writer); }

	void setSpecialDescription(const std::string& desc) { iAttributes->setStrAttr(ITEM_ATTRIBUTE_DESCRIPTION, desc); }
};

// #endif
