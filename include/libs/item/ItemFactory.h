#ifndef PR_ITEM_FACTORY_H
#define PR_ITEM_FACTORY_H

#include "libs/item/Item.h"

class ItemFactory
{
public:
	static Item* create(const uint16_t type, uint16_t count);
	static Item* create(PropStream& propStream);
};

#endif
