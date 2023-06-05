#ifndef PR_ITEMTYPE_H
#define PR_ITEMTYPE_H

#include "../../src/condition.h"
#include "../../src/position.h"
#include "constants/const.h"
#include "constants/enums.h"
#include "libs/item/itemenums.h"
#include "libs/util/xml/XMLElementBuilder.h"

#include <array>
#include <cstdint>
#include <unique_ptr.h>

struct Abilities
{
	uint32_t healthGain = 0;
	uint32_t healthTicks = 0;
	uint32_t manaGain = 0;
	uint32_t manaTicks = 0;

	uint32_t conditionImmunities = 0;
	uint32_t conditionSuppressions = 0;

	// stats modifiers
	std::array<int32_t, STAT_LAST + 1> stats = {0};
	std::array<int32_t, STAT_LAST + 1> statsPercent = {0};

	// extra skill modifiers
	std::array<int32_t, SKILL_LAST + 1> skills = {0};
	std::array<int32_t, SPECIALSKILL_LAST + 1> specialSkills = {0};
	std::array<int16_t, COMBAT_COUNT> specialMagicLevelSkill = {0};
	int32_t speed = 0;

	// field damage abilities modifiers
	std::array<int16_t, COMBAT_COUNT> fieldAbsorbPercent = {0};

	// damage abilities modifiers
	std::array<int16_t, COMBAT_COUNT> absorbPercent = {0};

	std::array<Reflect, COMBAT_COUNT> reflect;

	int16_t boostPercent[COMBAT_COUNT] = {0};

	// elemental damage
	uint16_t elementDamage = 0;
	CombatType_t elementType = COMBAT_NONE;

	bool manaShield = false;
	bool invisible = false;
	bool regeneration = false;
};

class ItemType final : virtual XMLElementBuilder<ItemType*>
{
public:
	ItemType() = default;
	ItemType(uint16_t id) : id(id){};

	// non-copyable
	ItemType(const ItemType& other) = delete;
	ItemType& operator=(const ItemType& other) = delete;

	ItemType(ItemType&& other) = default;
	ItemType& operator=(ItemType&& other) = default;

	ItemGroups group = ITEM_GROUP_NONE;
	ItemTypes type = ITEM_TYPE_NONE;
	uint16_t id = 0;
	uint16_t clientId = 0;
	bool stackable = false;
	bool isAnimation = false;

	std::string name;
	std::string article;
	std::string pluralName;
	std::string description;
	std::string runeSpellName;
	std::string vocationString;

	std::unique_ptr<Abilities> abilities;
	std::unique_ptr<ConditionDamage> conditionDamage;

	uint32_t attackSpeed = 0;
	uint32_t weight = 0;
	uint32_t levelDoor = 0;
	uint32_t decayTime = 0;
	uint32_t wieldInfo = 0;
	uint32_t minReqLevel = 0;
	uint32_t minReqMagicLevel = 0;
	uint32_t charges = 0;
	int32_t maxHitChance = -1;
	int32_t decayTo = -1;
	int32_t attack = 0;
	int32_t defense = 0;
	int32_t extraDefense = 0;
	int32_t armor = 0;
	uint16_t rotateTo = 0;
	int32_t runeMagLevel = 0;
	int32_t runeLevel = 0;
	uint64_t worth = 0;

	CombatType_t combatType = COMBAT_NONE;

	uint16_t transformToOnUse[2] = {0, 0};
	uint16_t transformToFree = 0;
	uint16_t destroyTo = 0;
	uint16_t maxTextLen = 0;
	uint16_t writeOnceItemId = 0;
	uint16_t transformEquipTo = 0;
	uint16_t transformDeEquipTo = 0;
	uint16_t maxItems = 8;
	uint16_t slotPosition = SLOTP_HAND;
	uint16_t speed = 0;
	uint16_t wareId = 0;

	MagicEffectClasses magicEffect = CONST_ME_NONE;
	Direction bedPartnerDir = DIRECTION_NONE;
	WeaponType_t weaponType = WEAPON_NONE;
	Ammo_t ammoType = AMMO_NONE;
	ShootType_t shootType = CONST_ANI_NONE;
	RaceType_t corpseType = RACE_NONE;
	FluidTypes_t fluidSource = FLUID_NONE;

	uint8_t floorChange = 0;
	uint8_t alwaysOnTopOrder = 0;
	uint8_t lightLevel = 0;
	uint8_t lightColor = 0;
	uint8_t shootRange = 1;
	uint8_t classification = 0;
	int8_t hitChance = 0;

	bool storeItem = false;
	bool forceUse = false;
	bool forceSerialize = false;
	bool hasHeight = false;
	bool walkStack = true;
	bool blockSolid = false;
	bool blockPickupable = false;
	bool blockProjectile = false;
	bool blockPathFind = false;
	bool allowPickupable = false;
	bool showDuration = false;
	bool showCharges = false;
	bool showAttributes = false;
	bool replaceable = true;
	bool pickupable = false;
	bool rotatable = false;
	bool useable = false;
	bool moveable = false;
	bool alwaysOnTop = false;
	bool canReadText = false;
	bool canWriteText = false;
	bool isVertical = false;
	bool isHorizontal = false;
	bool isHangable = false;
	bool allowDistRead = false;
	bool lookThrough = false;
	bool stopTime = false;
	bool showCount = true;
	bool supply = false;

	ItemType* loadFromXMLNode(pugi::xml_node node, bool reloading) override;

	bool isGroundTile() const { return group == ITEM_GROUP_GROUND; }
	bool isContainer() const { return group == ITEM_GROUP_CONTAINER; }
	bool isSplash() const { return group == ITEM_GROUP_SPLASH; }
	bool isFluidContainer() const { return group == ITEM_GROUP_FLUID; }

	bool isDoor() const { return (type == ITEM_TYPE_DOOR); }
	bool isMagicField() const { return (type == ITEM_TYPE_MAGICFIELD); }
	bool isTeleport() const { return (type == ITEM_TYPE_TELEPORT); }
	bool isKey() const { return (type == ITEM_TYPE_KEY); }
	bool isDepot() const { return (type == ITEM_TYPE_DEPOT); }
	bool isMailbox() const { return (type == ITEM_TYPE_MAILBOX); }
	bool isTrashHolder() const { return (type == ITEM_TYPE_TRASHHOLDER); }
	bool isBed() const { return (type == ITEM_TYPE_BED); }
	bool isRune() const { return (type == ITEM_TYPE_RUNE); }
	bool isPodium() const { return (type == ITEM_TYPE_PODIUM); }
	bool isPickupable() const { return (allowPickupable || pickupable); }
	bool isUseable() const { return (useable); }
	bool hasSubType() const { return (isFluidContainer() || isSplash() || stackable || charges != 0); }
	bool isSupply() const { return supply; }

	Abilities& getAbilities()
	{
		if (!abilities) {
			abilities.reset(new Abilities());
		}
		return *abilities;
	}

	std::string getPluralName() const
	{
		if (!pluralName.empty()) {
			return pluralName;
		}

		if (showCount == 0) {
			return name;
		}

		if (name.empty() || name.back() == 's') {
			return name;
		}

		std::string str;
		str.reserve(name.length() + 1);
		str.assign(name);
		str += 's';
		return str;
	}

	void setGroup(ItemGroups group)
	{
		this->group = group;
		switch (group) {
			case ITEM_GROUP_CONTAINER:
				this->type = ITEM_TYPE_CONTAINER;
				break;
			case ITEM_GROUP_DOOR:
				// not used
				this->type = ITEM_TYPE_DOOR;
				break;
			case ITEM_GROUP_MAGICFIELD:
				// not used
				this->type = ITEM_TYPE_MAGICFIELD;
				break;
			case ITEM_GROUP_TELEPORT:
				// not used
				this->type = ITEM_TYPE_TELEPORT;
				break;
			case ITEM_GROUP_NONE:
			case ITEM_GROUP_GROUND:
			case ITEM_GROUP_SPLASH:
			case ITEM_GROUP_FLUID:
			case ITEM_GROUP_CHARGES:
			case ITEM_GROUP_DEPRECATED:
				break;
			case ITEM_GROUP_PODIUM:
				this->type = ITEM_TYPE_PODIUM;
				break;
			default:
				break;
		}
	}

	void setType(const std::string& type)
	{
		auto itemType = ItemTypesMap->get(type);
		if (!ItemTypesMap->isNone(itemType)) {
			this->type = itemType;
			if (this->type == ITEM_TYPE_CONTAINER) {
				this->group = ITEM_GROUP_CONTAINER;
			}
		} else {
			std::cout << "[Warning - ItemType::setType] Unknown item type: " << type << std::endl;
		}
	}

	void setAttackSpeed(uint32_t attackSpeed)
	{
		if (attackSpeed > 0 && attackSpeed < 100) {
			std::cout << "[Warning - ItemType::setAttackSpeed]AttackSpeed lower than 100 for item: " << id << std::endl;
			attackSpeed = 100;
		}
		this->attackSpeed = attackSpeed;
	}

	void setFloorChange(const std::string& floorChange)
	{
		auto tileState = TileStatesMap->get(floorChange);
		if (!TileStatesMap->isNone(tileState)) {
			this->floorChange |= tileState;
		} else {
			std::cout << "[Warning - ItemType::setFloorChange] Unknown floorChange: " << floorChange << std::endl;
		}
	};

	void setCorpseType(const std::string& corpseType)
	{
		auto race = RaceTypesMap->get(corpseType);
		if (!RaceTypesMap->isNone(race)) {
			this->corpseType = race;
		} else {
			std::cout << "[Warning - ItemType::setCorpseType] Unknown corpseType: " << corpseType << std::endl;
		}
	}

	void setFluidSource(const std::string& fluidSource)
	{
		auto fluid = FluidTypesMap->get(fluidSource);
		if (!FluidTypesMap->isNone(fluid)) {
			this->fluidSource = fluid;
		} else {
			std::cout << "[Warning - ItemType::setFluidSource] Unknown fluidSource: " << fluidSource << std::endl;
		}
	}

	void setWeaponType(const std::string& weaponType)
	{
		auto weapon = WeaponTypesMap->get(weaponType);
		if (!WeaponTypesMap->isNone(weapon)) {
			this->weaponType = weapon;
		} else {
			std::cout << "[Warning - ItemType::setWeaponType] Unknown weaponType: " << weaponType << std::endl;
		}
	}

	void setSlotType(const std::string& slotType)
	{
		auto tmpStrValue = boost::algorithm::to_lower_copy<std::string>(slotType);
		if (tmpStrValue == "head") {
			this->slotPosition |= SLOTP_HEAD;
		} else if (tmpStrValue == "body") {
			this->slotPosition |= SLOTP_ARMOR;
		} else if (tmpStrValue == "legs") {
			this->slotPosition |= SLOTP_LEGS;
		} else if (tmpStrValue == "feet") {
			this->slotPosition |= SLOTP_FEET;
		} else if (tmpStrValue == "backpack") {
			this->slotPosition |= SLOTP_BACKPACK;
		} else if (tmpStrValue == "two-handed") {
			this->slotPosition |= SLOTP_TWO_HAND;
		} else if (tmpStrValue == "right-hand") {
			this->slotPosition &= ~SLOTP_LEFT;
		} else if (tmpStrValue == "left-hand") {
			this->slotPosition &= ~SLOTP_RIGHT;
		} else if (tmpStrValue == "necklace") {
			this->slotPosition |= SLOTP_NECKLACE;
		} else if (tmpStrValue == "ring") {
			this->slotPosition |= SLOTP_RING;
		} else if (tmpStrValue == "ammo") {
			this->slotPosition |= SLOTP_AMMO;
		} else if (tmpStrValue == "hand") {
			this->slotPosition |= SLOTP_HAND;
		} else {
			std::cout << "[Warning - ItemType::setSlotType] Unknown slotType: " << slotType << std::endl;
		}
	}
};

#endif
