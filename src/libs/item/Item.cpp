#include "libs/item/Item.h"

#include "../../container.h"
#include "../../game.h"
#include "libs/item/Items.h"

extern Game g_game;

Item::Item(const uint16_t type, uint16_t count) : id(type)
{
	const ItemType* it = Items::getInstance().getItemType(id);

	if (it->isFluidContainer() || it->isSplash()) {
		setFluidType(count);
	} else if (it->stackable) {
		if (count != 0) {
			setItemCount(count);
		} else if (it->charges != 0) {
			setItemCount(it->charges);
		}
	} else if (it->charges != 0) {
		if (count != 0) {
			setCharges(count);
		} else {
			setCharges(it->charges);
		}
	}

	setDefaultDuration();
}

Item::Item(const Item& i) : Thing(), id(i.id), count(i.count), loadedFromMap(i.loadedFromMap)
{
	if (i.iAttributes) {
		iAttributes.reset(new ItemAttributes(*i.iAttributes));
	}
}

Item* Item::clone() const
{
	Item* item = Item::CreateItem(id, count);
	if (iAttributes) {
		item->iAttributes.reset(new ItemAttributes(*iAttributes));
		if (item->getDuration() > 0) {
			item->incrementReferenceCounter();
			item->setDecaying(DECAYING_TRUE);
			g_game.toDecayItems.push_front(item);
		}
	}
	return item;
}

bool Item::equals(const Item* otherItem) const
{
	if (!otherItem || id != otherItem->id) {
		return false;
	}

	const auto& otherAttributes = otherItem->iAttributes;
	if (!iAttributes) {
		return !otherAttributes || otherAttributes->isUndefined();
	} else if (!otherAttributes) {
		return (iAttributes->isUndefined());
	}

	return otherAttributes->equals(*iAttributes);
}

Container* Item::CreateItemAsContainer(const uint16_t type, uint16_t size)
{
	auto it = Items::getInstance().getItemType(type);
	if (it->id == 0 || it->group == ITEM_GROUP_DEPRECATED || it->stackable || it->useable || it->moveable ||
	    it->pickupable || it->isDepot() || it->isSplash() || it->isDoor()) {
		return nullptr;
	}

	Container* newItem = new Container(type, size);
	newItem->incrementReferenceCounter();
	return newItem;
}
