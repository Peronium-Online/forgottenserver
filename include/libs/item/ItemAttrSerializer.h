#ifndef PR_ITEM_ATTR_SERIALIZER_H
#define PR_ITEM_ATTR_SERIALIZER_H

#include "libs/item/Item.h"
#include "libs/util/tools/propstream.h"

class ItemAttrSerializer
{
public:
	static bool unserializeAttr(Item* item, PropStream& propStream);
	static bool serializeAttr(const Item* item, PropWriteStream& propStream);
};

#endif
