// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "podium.h"

#include "game.h"

extern Game g_game;

void Podium::setAttributeFromPropStream(ItemAttrTypesIndex idx, PropStream& stream)
{
	if (idx == ATTR_PODIUMOUTFIT) {
		if (stream.size() < 15) {
			throw ItemAttrError{ATTR_PODIUMOUTFIT, "Unable to read podium outfit attribute"};
		}

		uint8_t flags;
		stream.read<uint8_t>(flags);
		setFlags(flags);

		uint8_t newDirection;
		stream.read<uint8_t>(newDirection);
		setDirection(static_cast<Direction>(newDirection));

		Look newOutfit;
		stream.read<uint16_t>(newOutfit.type);
		stream.read<uint8_t>(newOutfit.head);
		stream.read<uint8_t>(newOutfit.body);
		stream.read<uint8_t>(newOutfit.legs);
		stream.read<uint8_t>(newOutfit.feet);
		stream.read<uint8_t>(newOutfit.addons);
		stream.read<uint16_t>(newOutfit.mount);
		stream.read<uint8_t>(newOutfit.mountHead);
		stream.read<uint8_t>(newOutfit.mountBody);
		stream.read<uint8_t>(newOutfit.mountLegs);
		stream.read<uint8_t>(newOutfit.mountFeet);
		setOutfit(newOutfit);

		g_game.updatePodium(this);
		return;
	}

	return Item::setAttributeFromPropStream(idx, stream);
}
