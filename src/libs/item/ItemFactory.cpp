#include "libs/item/ItemFactory.h"

#include "../../bed.h"
#include "../../combat.h"
#include "../../container.h"
#include "../../depotlocker.h"
#include "../../house.h"
#include "../../mailbox.h"
#include "../../podium.h"
#include "../../teleport.h"
#include "../../trashholder.h"
#include "libs/item/ItemAttrSerializer.h"
#include "libs/item/Items.h"

Item* ItemFactory::create(const uint16_t type, uint16_t count)
{
	Item* newItem = nullptr;

	auto it = Items::getInstance().getItemType(type);
	if (it->group == ITEM_GROUP_DEPRECATED) {
		return nullptr;
	}

	if (it->stackable && count == 0) {
		count = 1;
	}

	if (it->id != 0) {
		if (it->isDepot()) {
			newItem = new DepotLocker(type);
		} else if (it->isContainer()) {
			newItem = new Container(type);
		} else if (it->isTeleport()) {
			newItem = new Teleport(type);
		} else if (it->isMagicField()) {
			newItem = new MagicField(type);
		} else if (it->isDoor()) {
			newItem = new Door(type);
		} else if (it->isTrashHolder()) {
			newItem = new TrashHolder(type);
		} else if (it->isMailbox()) {
			newItem = new Mailbox(type);
		} else if (it->isBed()) {
			newItem = new BedItem(type);
		} else if (it->isPodium()) {
			newItem = new Podium(type);
		} else {
			newItem = new Item(type, count);
		}

		newItem->incrementReferenceCounter();
	}

	return newItem;
}

Item* ItemFactory::create(PropStream& propStream)
{
	uint16_t id;
	if (!propStream.read<uint16_t>(id)) {
		return nullptr;
	}

	switch (id) {
		case ITEM_FIREFIELD_PVP_FULL:
			id = ITEM_FIREFIELD_PERSISTENT_FULL;
			break;

		case ITEM_FIREFIELD_PVP_MEDIUM:
			id = ITEM_FIREFIELD_PERSISTENT_MEDIUM;
			break;

		case ITEM_FIREFIELD_PVP_SMALL:
			id = ITEM_FIREFIELD_PERSISTENT_SMALL;
			break;

		case ITEM_ENERGYFIELD_PVP:
			id = ITEM_ENERGYFIELD_PERSISTENT;
			break;

		case ITEM_POISONFIELD_PVP:
			id = ITEM_POISONFIELD_PERSISTENT;
			break;

		case ITEM_MAGICWALL:
			id = ITEM_MAGICWALL_PERSISTENT;
			break;

		case ITEM_WILDGROWTH:
			id = ITEM_WILDGROWTH_PERSISTENT;
			break;

		default:
			break;
	}

	auto newItem = create(id, 0);
	ItemAttrSerializer::unserializeAttr(newItem, propStream);

	return newItem;
}

Container* ItemFactory::createAsContainer(const uint16_t type, uint16_t size)
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
