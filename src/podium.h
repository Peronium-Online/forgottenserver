// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_PODIUM_H
#define FS_PODIUM_H

#include "libs/item/Item.h"

class Podium final : public Item
{
public:
	explicit Podium(uint16_t type) : Item(type){};

	static bool isPodium(const Item* item) { return item->getType() == ITEM_TYPE_PODIUM; }

	static Podium* toPodium(Item* item) { return static_cast<Podium*>(item); }
	static const Podium* toPodium(const Item* item) { return static_cast<const Podium*>(item); }

	void setAttributeFromPropStream(ItemAttrTypesIndex idx, PropStream& stream) override;

	void setOutfit(const Look& newOutfit) { outfit = newOutfit; }
	const Look getOutfit() const { return outfit; }

	bool hasFlag(PodiumFlags flag) const { return flags.test(flag); }
	void setFlagValue(PodiumFlags flag, bool value)
	{
		if (value) {
			flags.set(flag);
		} else {
			flags.reset(flag);
		}
	}
	void setFlags(uint8_t newFlags) { flags = newFlags; }
	std::bitset<3> getFlags() const { return flags; }

	const Direction getDirection() const { return direction; }
	void setDirection(Direction newDirection) { direction = newDirection; }

protected:
	Look outfit;

private:
	std::bitset<3> flags = {true}; // show platform only
	Direction direction = DIRECTION_SOUTH;
};

#endif // FS_PODIUM_H
