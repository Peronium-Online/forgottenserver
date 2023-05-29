#include "libs/item/Items.h"

#include "libs/item/itemenums.h"
#include "libs/util/tools/bitwise.h"

bool Items::load(const OTBNode& node, PropStream stream)
{
	uint32_t flags;
	if (!stream.read<uint32_t>(flags)) {
		return false;
	}

	uint16_t serverId = 0;
	uint16_t clientId = 0;
	uint16_t speed = 0;
	uint16_t wareId = 0;
	uint8_t lightLevel = 0;
	uint8_t lightColor = 0;
	uint8_t alwaysOnTopOrder = 0;
	uint8_t classification = 0;

	uint8_t attrib;
	while (stream.read<uint8_t>(attrib)) {
		uint16_t datalen;
		if (!stream.read<uint16_t>(datalen)) {
			return false;
		}

		switch (attrib) {
			case ITEM_ATTR_SERVERID: {
				if (datalen != sizeof(uint16_t)) {
					return false;
				}

				if (!stream.read<uint16_t>(serverId)) {
					return false;
				}
				break;
			}

			case ITEM_ATTR_CLIENTID: {
				if (datalen != sizeof(uint16_t)) {
					return false;
				}

				if (!stream.read<uint16_t>(clientId)) {
					return false;
				}
				break;
			}

			case ITEM_ATTR_SPEED: {
				if (datalen != sizeof(uint16_t)) {
					return false;
				}

				if (!stream.read<uint16_t>(speed)) {
					return false;
				}
				break;
			}

			case ITEM_ATTR_LIGHT2: {
				if (datalen != sizeof(lightBlock2)) {
					return false;
				}

				lightBlock2 lb2;
				if (!stream.read(lb2)) {
					return false;
				}

				lightLevel = static_cast<uint8_t>(lb2.lightLevel);
				lightColor = static_cast<uint8_t>(lb2.lightColor);
				break;
			}

			case ITEM_ATTR_TOPORDER: {
				if (datalen != sizeof(uint8_t)) {
					return false;
				}

				if (!stream.read<uint8_t>(alwaysOnTopOrder)) {
					return false;
				}
				break;
			}

			case ITEM_ATTR_WAREID: {
				if (datalen != sizeof(uint16_t)) {
					return false;
				}

				if (!stream.read<uint16_t>(wareId)) {
					return false;
				}
				break;
			}

			case ITEM_ATTR_CLASSIFICATION: {
				if (datalen != sizeof(uint8_t)) {
					return false;
				}

				if (!stream.read<uint8_t>(classification)) {
					return false;
				}
				break;
			}

			default: {
				// skip unknown attributes
				if (!stream.skip(datalen)) {
					return false;
				}
				break;
			}
		}
	}

	clientIdToServerIdMap.emplace(clientId, serverId);

	// store the found item
	ItemType* iType = new ItemType(serverId);
	iType->setGroup(static_cast<ItemGroups>(node.type));
	iType->blockSolid = hasBitSet(FLAG_BLOCK_SOLID, flags);
	iType->blockProjectile = hasBitSet(FLAG_BLOCK_PROJECTILE, flags);
	iType->blockPathFind = hasBitSet(FLAG_BLOCK_PATHFIND, flags);
	iType->hasHeight = hasBitSet(FLAG_HAS_HEIGHT, flags);
	iType->useable = hasBitSet(FLAG_USEABLE, flags);
	iType->pickupable = hasBitSet(FLAG_PICKUPABLE, flags);
	iType->moveable = hasBitSet(FLAG_MOVEABLE, flags);
	iType->stackable = hasBitSet(FLAG_STACKABLE, flags);

	iType->alwaysOnTop = hasBitSet(FLAG_ALWAYSONTOP, flags);
	iType->isVertical = hasBitSet(FLAG_VERTICAL, flags);
	iType->isHorizontal = hasBitSet(FLAG_HORIZONTAL, flags);
	iType->isHangable = hasBitSet(FLAG_HANGABLE, flags);
	iType->allowDistRead = hasBitSet(FLAG_ALLOWDISTREAD, flags);
	iType->rotatable = hasBitSet(FLAG_ROTATABLE, flags);
	iType->canReadText = hasBitSet(FLAG_READABLE, flags);
	iType->lookThrough = hasBitSet(FLAG_LOOKTHROUGH, flags);
	iType->isAnimation = hasBitSet(FLAG_ANIMATION, flags);
	// iType->walkStack = !hasBitSet(FLAG_FULLTILE, flags);
	iType->forceUse = hasBitSet(FLAG_FORCEUSE, flags);

	iType->clientId = clientId;
	iType->speed = speed;
	iType->lightLevel = lightLevel;
	iType->lightColor = lightColor;
	iType->wareId = wareId;
	iType->classification = classification;
	iType->alwaysOnTopOrder = alwaysOnTopOrder;

	this->addItem(*iType);
}

bool Items::loadFromOTB()
{
	OTBLoadable::loadFromOTB();

	this->items.shrink_to_fit();
}
