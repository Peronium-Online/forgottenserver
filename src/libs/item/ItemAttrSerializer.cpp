#include "libs/item/ItemAttrSerializer.h"

bool ItemAttrSerializer::unserializeAttr(Item* item, PropStream& propStream)
{
	uint8_t attr_type;
	while (propStream.read<uint8_t>(attr_type) && attr_type != 0) {
		item->setAttributeFromPropStream(static_cast<ItemAttrTypesIndex>(attr_type), propStream);
	}
	return true;
}

bool ItemAttrSerializer::serializeAttr(const Item* item, PropWriteStream& propWriteStream)
{
	if (item->isStackable() || item->isFluidContainer() || item->isSplash()) {
		propWriteStream.write<uint8_t>(ATTR_COUNT);
		propWriteStream.write<uint8_t>(item->getSubType());
	}

	uint16_t charges = item->getCharges();
	if (charges != 0) {
		propWriteStream.write<uint8_t>(ATTR_CHARGES);
		propWriteStream.write<uint16_t>(charges);
	}

	if (item->isMoveable()) {
		uint16_t actionId = item->getActionId();
		if (actionId != 0) {
			propWriteStream.write<uint8_t>(ATTR_ACTION_ID);
			propWriteStream.write<uint16_t>(actionId);
		}
	}

	const std::string& text = item->getText();
	if (!text.empty()) {
		propWriteStream.write<uint8_t>(ATTR_TEXT);
		propWriteStream.writeString(text);
	}

	const time_t writtenDate = item->getDate();
	if (writtenDate != 0) {
		propWriteStream.write<uint8_t>(ATTR_WRITTENDATE);
		propWriteStream.write<uint32_t>(writtenDate);
	}

	const std::string& writer = item->getWriter();
	if (!writer.empty()) {
		propWriteStream.write<uint8_t>(ATTR_WRITTENBY);
		propWriteStream.writeString(writer);
	}

	const std::string& specialDesc = item->getSpecialDescription();
	if (!specialDesc.empty()) {
		propWriteStream.write<uint8_t>(ATTR_DESC);
		propWriteStream.writeString(specialDesc);
	}

	auto duration = item->getDuration();
	if (duration != 0) {
		propWriteStream.write<uint8_t>(ATTR_DURATION);
		propWriteStream.write<uint32_t>(duration);
	}

	ItemDecayState decayState = item->getDecaying();
	if (decayState == DECAYING_TRUE || decayState == DECAYING_PENDING) {
		propWriteStream.write<uint8_t>(ATTR_DECAYING_STATE);
		propWriteStream.write<uint8_t>(decayState);
	}

	auto name = item->getName();
	if (name != "") {
		propWriteStream.write<uint8_t>(ATTR_NAME);
		propWriteStream.writeString(name);
	}

	auto article = item->getArticle();
	if (article != "") {
		propWriteStream.write<uint8_t>(ATTR_ARTICLE);
		propWriteStream.writeString(article);
	}

	auto pluralName = item->getPluralName();
	if (pluralName != "") {
		propWriteStream.write<uint8_t>(ATTR_PLURALNAME);
		propWriteStream.writeString(pluralName);
	}

	auto weight = item->getWeight();
	if (weight != 0) {
		propWriteStream.write<uint8_t>(ATTR_WEIGHT);
		propWriteStream.write<uint32_t>(weight);
	}

	auto attack = item->getAttack();
	if (attack != 0) {
		propWriteStream.write<uint8_t>(ATTR_ATTACK);
		propWriteStream.write<int32_t>(attack);
	}

	auto attackSpeed = item->getAttackSpeed();
	if (attackSpeed != 0) {
		propWriteStream.write<uint8_t>(ATTR_ATTACK_SPEED);
		propWriteStream.write<uint32_t>(attackSpeed);
	}

	auto defense = item->getDefense();
	if (defense != 0) {
		propWriteStream.write<uint8_t>(ATTR_DEFENSE);
		propWriteStream.write<int32_t>(defense);
	}

	auto extraDefense = item->getExtraDefense();
	if (extraDefense != 0) {
		propWriteStream.write<uint8_t>(ATTR_EXTRADEFENSE);
		propWriteStream.write<int32_t>(extraDefense);
	}

	auto armor = item->getArmor();
	if (armor != 0) {
		propWriteStream.write<uint8_t>(ATTR_ARMOR);
		propWriteStream.write<int32_t>(armor);
	}

	auto hitChance = item->getHitChance();
	if (hitChance != 0) {
		propWriteStream.write<uint8_t>(ATTR_HITCHANCE);
		propWriteStream.write<int8_t>(hitChance);
	}

	auto shootRange = item->getShootRange();
	if (shootRange != 0) {
		propWriteStream.write<uint8_t>(ATTR_SHOOTRANGE);
		propWriteStream.write<uint8_t>(shootRange);
	}

	auto decayTo = item->getDecayTo();
	if (decayTo != 0) {
		propWriteStream.write<uint8_t>(ATTR_DECAYTO);
		propWriteStream.write<int32_t>(decayTo);
	}

	auto wrapId = item->getWrapId();
	if (wrapId != 0) {
		propWriteStream.write<uint8_t>(ATTR_WRAPID);
		propWriteStream.write<uint16_t>(wrapId);
	}

	if (item->isStoreItem()) {
		propWriteStream.write<uint8_t>(ATTR_STOREITEM);
		propWriteStream.write<uint8_t>(true);
	}

	if (item->isOpenContainer()) {
		propWriteStream.write<uint8_t>(ATTR_OPENCONTAINER);
		propWriteStream.write<uint8_t>(true);
	}

	if (item->hasCustomAttribute()) {
		const auto customAttrMap = item->getCustomAttributeMap();
		propWriteStream.write<uint8_t>(ATTR_CUSTOM_ATTRIBUTES);
		propWriteStream.write<uint64_t>(static_cast<uint64_t>(customAttrMap->size()));
		for (const auto& entry : *customAttrMap) {
			// Serializing key type and value
			propWriteStream.writeString(entry.first);

			// Serializing value type and value
			entry.second.serialize(propWriteStream);
		}
	}

	const auto& reflects = item->getReflects();
	if (!reflects.empty()) {
		propWriteStream.write<uint8_t>(ATTR_REFLECT);
		propWriteStream.write<uint16_t>(reflects.size());

		for (const auto& reflect : reflects) {
			propWriteStream.write<CombatType_t>(reflect.first);
			propWriteStream.write<uint16_t>(reflect.second.percent);
			propWriteStream.write<uint16_t>(reflect.second.chance);
		}
	}

	const auto& boosts = item->getBoosts();
	if (!boosts.empty()) {
		propWriteStream.write<uint8_t>(ATTR_BOOST);
		propWriteStream.write<uint16_t>(boosts.size());

		for (const auto& boost : boosts) {
			propWriteStream.write<CombatType_t>(boost.first);
			propWriteStream.write<uint16_t>(boost.second);
		}
	}
}
