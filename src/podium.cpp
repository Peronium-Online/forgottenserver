// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "podium.h"

#include "game.h"

extern Game g_game;

Attr_ReadValue Podium::readAttr(AttrTypes_t attr, PropStream& propStream)
{
	switch (attr) {
		case ATTR_PODIUMOUTFIT: {
			if (propStream.size() < 15) {
				return ATTR_READ_ERROR;
			}

			uint8_t flags;
			propStream.read<uint8_t>(flags);
			setFlags(flags);

			uint8_t newDirection;
			propStream.read<uint8_t>(newDirection);
			setDirection(static_cast<Direction>(newDirection));

			Look newOutfit;
			propStream.read<uint16_t>(newOutfit.type);
			propStream.read<uint8_t>(newOutfit.head);
			propStream.read<uint8_t>(newOutfit.body);
			propStream.read<uint8_t>(newOutfit.legs);
			propStream.read<uint8_t>(newOutfit.feet);
			propStream.read<uint8_t>(newOutfit.addons);
			propStream.read<uint16_t>(newOutfit.mount);
			propStream.read<uint8_t>(newOutfit.mountHead);
			propStream.read<uint8_t>(newOutfit.mountBody);
			propStream.read<uint8_t>(newOutfit.mountLegs);
			propStream.read<uint8_t>(newOutfit.mountFeet);
			setOutfit(newOutfit);

			g_game.updatePodium(this);
			return ATTR_READ_CONTINUE;
		}

		default:
			break;
	}
	return Item::readAttr(attr, propStream);
}

void Podium::serializeAttr(PropWriteStream& propWriteStream) const
{
	if (ATTR_PODIUMOUTFIT != 0) {
		propWriteStream.write<uint8_t>(ATTR_PODIUMOUTFIT);
		propWriteStream.write<uint8_t>(static_cast<uint8_t>(flags.to_ulong()));
		propWriteStream.write<uint8_t>(direction);
		propWriteStream.write<uint16_t>(outfit.type);
		propWriteStream.write<uint8_t>(outfit.head);
		propWriteStream.write<uint8_t>(outfit.body);
		propWriteStream.write<uint8_t>(outfit.legs);
		propWriteStream.write<uint8_t>(outfit.feet);
		propWriteStream.write<uint8_t>(outfit.addons);
		propWriteStream.write<uint16_t>(outfit.mount);
		propWriteStream.write<uint8_t>(outfit.mountHead);
		propWriteStream.write<uint8_t>(outfit.mountBody);
		propWriteStream.write<uint8_t>(outfit.mountLegs);
		propWriteStream.write<uint8_t>(outfit.mountFeet);
	}
}
