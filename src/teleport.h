// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_TELEPORT_H
#define FS_TELEPORT_H

#include "cylinder.h"
#include "libs/item/Item.h"

class Teleport final : public Item, public Cylinder
{
public:
	explicit Teleport(uint16_t type) : Item(type){};

	static bool isTeleport(const Item* item) { return item->getType() == ITEM_TYPE_TELEPORT; }
	static Teleport* getTeleport(Item* item) { return dynamic_cast<Teleport*>(item); }

	void setAttributeFromPropStream(ItemAttrTypesIndex idx, PropStream& stream) override;

	const Position& getDestPos() const { return destPos; }
	void setDestPos(const Position& pos) { destPos = pos; }

	// cylinder implementations
	ReturnValue queryAdd(int32_t index, const Thing& thing, uint32_t count, uint32_t flags,
	                     Creature* actor = nullptr) const override;
	ReturnValue queryMaxCount(int32_t index, const Thing& thing, uint32_t count, uint32_t& maxQueryCount,
	                          uint32_t flags) const override;
	ReturnValue queryRemove(const Thing& thing, uint32_t count, uint32_t flags,
	                        Creature* actor = nullptr) const override;
	Cylinder* queryDestination(int32_t& index, const Thing& thing, Item** destItem, uint32_t& flags) override;

	void addThing(Thing* thing) override;
	void addThing(int32_t index, Thing* thing) override;

	void updateThing(Thing* thing, uint16_t itemId, uint32_t count) override;
	void replaceThing(uint32_t index, Thing* thing) override;

	void removeThing(Thing* thing, uint32_t count) override;

	void postAddNotification(Thing* thing, const Cylinder* oldParent, int32_t index,
	                         cylinderlink_t link = LINK_OWNER) override;
	void postRemoveNotification(Thing* thing, const Cylinder* newParent, int32_t index,
	                            cylinderlink_t link = LINK_OWNER) override;

private:
	Position destPos;
};

#endif // FS_TELEPORT_H
