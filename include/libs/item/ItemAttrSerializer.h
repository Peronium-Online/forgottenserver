#ifndef PR_ITEM_ATTR_SERIALIZER_H
#define PR_ITEM_ATTR_SERIALIZER_H

#include "../../../src/bed.h"
#include "../../../src/podium.h"
#include "../../../src/teleport.h"
#include "libs/item/Item.h"
#include "libs/util/tools/propstream.h"

class ItemAttrSerializer
{
public:
	static bool unserializeAttr(Item* item, PropStream& propStream);
	static bool serializeAttr(const Item* item, PropWriteStream& propStream);

	// TODO: move this if find a better place
	static bool serializeBedAttr(const BedItem* item, PropWriteStream& propStream);
	static bool serializePodiumAttr(const Podium* item, PropWriteStream& propStream);
	static bool serializeTeleportAttr(const Teleport* item, PropWriteStream& propStream);
};

#endif
