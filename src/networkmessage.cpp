// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "networkmessage.h"

#include "container.h"
#include "podium.h"

std::string NetworkMessage::getString(uint16_t stringLen /* = 0*/)
{
	if (stringLen == 0) {
		stringLen = get<uint16_t>();
	}

	if (!canRead(stringLen)) {
		return std::string();
	}

	char* v = reinterpret_cast<char*>(buffer) + info.position; // does not break strict aliasing
	info.position += stringLen;
	return std::string(v, stringLen);
}

Position NetworkMessage::getPosition()
{
	Position pos;
	pos.x = get<uint16_t>();
	pos.y = get<uint16_t>();
	pos.z = getByte();
	return pos;
}

void NetworkMessage::addString(const std::string& value)
{
	size_t stringLen = value.length();
	if (!canAdd(stringLen + 2) || stringLen > 8192) {
		return;
	}

	add<uint16_t>(stringLen);
	memcpy(buffer + info.position, value.c_str(), stringLen);
	info.position += stringLen;
	info.length += stringLen;
}

void NetworkMessage::addDouble(double value, uint8_t precision /* = 2*/)
{
	addByte(precision);
	add<uint32_t>(static_cast<uint32_t>((value * std::pow(static_cast<float>(10), precision)) +
	                                    std::numeric_limits<int32_t>::max()));
}

void NetworkMessage::addBytes(const char* bytes, size_t size)
{
	if (!canAdd(size) || size > 8192) {
		return;
	}

	memcpy(buffer + info.position, bytes, size);
	info.position += size;
	info.length += size;
}

void NetworkMessage::addPaddingBytes(size_t n)
{
	if (!canAdd(n)) {
		return;
	}

	memset(buffer + info.position, 0x33, n);
	info.length += n;
}

void NetworkMessage::addPosition(const Position& pos)
{
	add<uint16_t>(pos.x);
	add<uint16_t>(pos.y);
	addByte(pos.z);
}

void NetworkMessage::addItem(uint16_t id, uint8_t count)
{
	auto it = Items::getInstance().getItemType(id);

	add<uint16_t>(it->clientId);

	if (it->stackable) {
		addByte(count);
	} else if (it->isSplash() || it->isFluidContainer()) {
		addByte(fluidMap[count & 7]);
	} else if (it->isContainer()) {
		addByte(0x00); // assigned loot container icon
		addByte(0x00); // quiver ammo count
	} else if (it->classification > 0) {
		addByte(0x00); // item tier (0-10)
	}

	if (it->isPodium()) {
		add<uint16_t>(0); // looktype
		add<uint16_t>(0); // lookmount
		addByte(2);       // direction
		addByte(0x01);    // is visible (bool)
	}
}

void NetworkMessage::addItem(const Item* item)
{
	add<uint16_t>(item->getClientID());

	if (item->isStackable()) {
		addByte(std::min<uint16_t>(0xFF, item->getItemCount()));
	} else if (item->isSplash() || item->isFluidContainer()) {
		addByte(fluidMap[item->getFluidType() & 7]);
	} else if (item->getClassification() > 0) {
		addByte(0x00); // item tier (0-10)
	}

	if (item->isContainer()) {
		addByte(0x00); // assigned loot container icon
		// quiver ammo count
		const Container* container = item->getContainer();
		if (container && item->getWeaponType() == WEAPON_QUIVER) {
			addByte(0x01);
			add<uint32_t>(container->getAmmoCount());
		} else {
			addByte(0x00);
		}
	}

	// display outfit on the podium
	if (item->isPodium()) {
		const Podium* podium = Podium::toPodium(item);
		const Look& outfit = podium->getOutfit();

		// add outfit
		if (podium->hasFlag(PODIUM_SHOW_OUTFIT)) {
			add<uint16_t>(outfit.type);
			if (outfit.type != 0) {
				addByte(outfit.head);
				addByte(outfit.body);
				addByte(outfit.legs);
				addByte(outfit.feet);
				addByte(outfit.addons);
			}
		} else {
			add<uint16_t>(0);
		}

		// add mount
		if (podium->hasFlag(PODIUM_SHOW_MOUNT)) {
			add<uint16_t>(outfit.mount);
			if (outfit.mount != 0) {
				addByte(outfit.mountHead);
				addByte(outfit.mountBody);
				addByte(outfit.mountLegs);
				addByte(outfit.mountFeet);
			}
		} else {
			add<uint16_t>(0);
		}

		addByte(podium->getDirection());
		addByte(podium->hasFlag(PODIUM_SHOW_PLATFORM) ? 0x01 : 0x00);
		return;
	}
}

void NetworkMessage::addItemId(uint16_t itemId) { add<uint16_t>(Items::getInstance().getItemType(itemId)->clientId); }
