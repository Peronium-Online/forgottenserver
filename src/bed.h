// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_BED_H
#define FS_BED_H

#include "libs/item/Item.h"

class House;
class Player;

class BedItem final : public Item
{
public:
	explicit BedItem(uint16_t id);

	static bool isBedItem(Item* item) { return item->getType() == ITEM_TYPE_BED; }

	static BedItem* toBedItem(Item* item) { return static_cast<BedItem*>(item); }
	static const BedItem* toBedItem(const Item* item) { return static_cast<const BedItem*>(item); }

	void setAttributeFromPropStream(ItemAttrTypesIndex idx, PropStream& stream) override;
	void serializeAttr(PropWriteStream& propWriteStream) const override;

	bool canRemove() const override { return !house; }

	uint32_t getSleeper() const { return sleeperGUID; }

	House* getHouse() const { return house; }
	void setHouse(House* h) { house = h; }

	bool canUse(Player* player);

	bool trySleep(Player* player);
	bool sleep(Player* player);
	void wakeUp(Player* player);

	BedItem* getNextBedItem() const;

private:
	void updateAppearance(const Player* player);
	void regeneratePlayer(Player* player) const;
	void internalSetSleeper(const Player* player);
	void internalRemoveSleeper();

	House* house = nullptr;
	uint64_t sleepStart;
	uint32_t sleeperGUID;
};

#endif // FS_BED_H
