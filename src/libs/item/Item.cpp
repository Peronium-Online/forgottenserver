#include "libs/item/Item.h"

#include "libs/item/Items.h"

Item::Item(const uint16_t type, uint16_t count /*= 0*/) : id(type)
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
