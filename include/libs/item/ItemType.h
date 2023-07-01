#ifndef PR_ITEMTYPE_H
#define PR_ITEMTYPE_H

#include "../../../src/condition.h"
#include "../../../src/position.h"
#include "constants/const.h"
#include "libs/item/itemenums.h"
#include "libs/item/itemmaps.h"
#include "libs/util/tools/gamehelpers.h"
#include "libs/util/xml/XMLElementBuilder.h"

#include <array>
#include <cstdint>

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
	WeaponTypes weaponType = WEAPON_NONE;
	AmmoTypes ammoType = AMMO_NONE;
	ShootType_t shootType = CONST_ANI_NONE;
	RaceType_t corpseType = RACE_NONE;
	FluidTypes fluidSource = FLUID_NONE;

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

	void setAmmoType(const std::string& ammoType)
	{
		auto ammo = AmmoTypesMap->get(ammoType);
		if (!AmmoTypesMap->isNone(ammo)) {
			this->ammoType = ammo;
		} else {
			std::cout << "[Warning - ItemType::setAmmoType] Unknown ammoType: " << ammoType << std::endl;
		}
	}

	void setShootType(const std::string& shootType)
	{
		ShootType_t shoot = getShootType(boost::algorithm::to_lower_copy<std::string>(shootType));
		if (shoot != CONST_ANI_NONE) {
			this->shootType = shoot;
		} else {
			std::cout << "[Warning - ItemType::setShootType] Unknown shootType: " << shootType << std::endl;
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

	void setMagicEffect(const std::string& magicEffect)
	{
		MagicEffectClasses effect = getMagicEffect(boost::algorithm::to_lower_copy<std::string>(magicEffect));
		if (effect != CONST_ME_NONE) {
			this->magicEffect = effect;
		} else {
			std::cout << "[Warning - ItemType::setMagicEffect] Unknown effect: " << magicEffect << std::endl;
		}
	}

	void setHitChance(int8_t hitChance) { this->hitChance = std::min<int8_t>(100, std::max<int8_t>(-100, hitChance)); }

	void setMaxHitChance(uint8_t hitChance) { this->maxHitChance = std::min<uint8_t>(100, hitChance); }

	void setHealthGain(uint32_t gain)
	{
		this->abilities->regeneration = true;
		this->abilities->healthGain = gain;
	}

	void setHealthTicks(uint32_t ticks)
	{
		this->abilities->regeneration = true;
		this->abilities->healthTicks = ticks;
	}

	void setManaGain(uint32_t gain)
	{
		this->abilities->regeneration = true;
		this->abilities->manaGain = gain;
	}

	void setManaTicks(uint32_t ticks)
	{
		this->abilities->regeneration = true;
		this->abilities->manaTicks = ticks;
	}

	void setSwordSkill(int32_t skill) { this->abilities->skills[SKILL_SWORD] = skill; }
	void setAxeSkill(int32_t skill) { this->abilities->skills[SKILL_AXE] = skill; }
	void setClubSkill(int32_t skill) { this->abilities->skills[SKILL_CLUB] = skill; }
	void setDistSkill(int32_t skill) { this->abilities->skills[SKILL_DISTANCE] = skill; }
	void setFishSkill(int32_t skill) { this->abilities->skills[SKILL_FISHING] = skill; }
	void setShieldSkill(int32_t skill) { this->abilities->skills[SKILL_SHIELD] = skill; }
	void setFistSkill(int32_t skill) { this->abilities->skills[SKILL_FIST] = skill; }

	void setCriticalHitAmount(int32_t amount)
	{
		this->abilities->specialSkills[SPECIALSKILL_CRITICALHITAMOUNT] = amount;
	}
	void setCriticalHitChance(int32_t chance)
	{
		this->abilities->specialSkills[SPECIALSKILL_CRITICALHITCHANCE] = chance;
	}
	void setManaLeechAmount(int32_t amount) { this->abilities->specialSkills[SPECIALSKILL_MANALEECHAMOUNT] = amount; }
	void setManaLeechChance(int32_t chance) { this->abilities->specialSkills[SPECIALSKILL_MANALEECHCHANCE] = chance; }
	void setLifeLeechAmount(int32_t amount) { this->abilities->specialSkills[SPECIALSKILL_LIFELEECHAMOUNT] = amount; }
	void setLifeLeechChance(int32_t chance) { this->abilities->specialSkills[SPECIALSKILL_LIFELEECHCHANCE] = chance; }

	void setMaxHitPoints(int32_t max) { this->abilities->stats[STAT_MAXHITPOINTS] = max; }
	void setMaxHitPointsPercent(int32_t percent) { this->abilities->statsPercent[STAT_MAXHITPOINTS] = percent; }
	void setMaxManaPoints(int32_t max) { this->abilities->stats[STAT_MAXMANAPOINTS] = max; }
	void setMaxManaPointsPercent(int32_t percent) { this->abilities->statsPercent[STAT_MAXMANAPOINTS] = percent; }
	void setMagicPoints(int32_t max) { this->abilities->stats[STAT_MAGICPOINTS] = max; }
	void setMagicPointsPercent(int32_t percent) { this->abilities->statsPercent[STAT_MAGICPOINTS] = percent; }

	void setFieldAbsorbPercentEnergy(int16_t percent)
	{
		this->abilities->fieldAbsorbPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += percent;
	}
	void setFieldAbsorbPercentFire(int16_t percent)
	{
		this->abilities->fieldAbsorbPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += percent;
	}
	void setFieldAbsorbPercentEarth(int16_t percent)
	{
		this->abilities->fieldAbsorbPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += percent;
	}

	void setAbsorbPercent(int16_t percent)
	{
		for (auto& i : this->abilities->absorbPercent) {
			i += percent;
		}
	}
	void setAbsorbPercentElements(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += percent;
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += percent;
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += percent;
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_ICEDAMAGE)] += percent;
	}
	void setAbsorbPercentMagic(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += percent;
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += percent;
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += percent;
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_ICEDAMAGE)] += percent;
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_HOLYDAMAGE)] += percent;
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_DEATHDAMAGE)] += percent;
	}
	void setAbsorbPercentEnergy(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += percent;
	}
	void setAbsorbPercentFire(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += percent;
	}
	void setAbsorbPercentPhysical(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_PHYSICALDAMAGE)] += percent;
	}
	void setAbsorbPercentEarth(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += percent;
	}
	void setAbsorbPercentIce(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_ICEDAMAGE)] += percent;
	}
	void setAbsorbPercentHoly(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_HOLYDAMAGE)] += percent;
	}
	void setAbsorbPercentDeath(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_DEATHDAMAGE)] += percent;
	}
	void setAbsorbPercentLifeDrain(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_LIFEDRAIN)] += percent;
	}
	void setAbsorbPercentManaDrain(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_MANADRAIN)] += percent;
	}
	void setAbsorbPercentDrown(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_DROWNDAMAGE)] += percent;
	}
	void setAbsorbPercentHealing(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_HEALING)] += percent;
	}
	void setAbsorbPercentUndefined(int16_t percent)
	{
		this->abilities->absorbPercent[combatTypeToIndex(COMBAT_UNDEFINEDDAMAGE)] += percent;
	}

	void setReflectPercent(int16_t percent)
	{
		for (auto& i : this->abilities->reflect) {
			i.percent += percent;
		}
	}
	void setReflectPercentElements(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_ENERGYDAMAGE)].percent += percent;
		this->abilities->reflect[combatTypeToIndex(COMBAT_FIREDAMAGE)].percent += percent;
		this->abilities->reflect[combatTypeToIndex(COMBAT_EARTHDAMAGE)].percent += percent;
		this->abilities->reflect[combatTypeToIndex(COMBAT_ICEDAMAGE)].percent += percent;
	}
	void setReflectPercentMagic(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_ENERGYDAMAGE)].percent += percent;
		this->abilities->reflect[combatTypeToIndex(COMBAT_FIREDAMAGE)].percent += percent;
		this->abilities->reflect[combatTypeToIndex(COMBAT_EARTHDAMAGE)].percent += percent;
		this->abilities->reflect[combatTypeToIndex(COMBAT_ICEDAMAGE)].percent += percent;
		this->abilities->reflect[combatTypeToIndex(COMBAT_HOLYDAMAGE)].percent += percent;
		this->abilities->reflect[combatTypeToIndex(COMBAT_DEATHDAMAGE)].percent += percent;
	}
	void setReflectPercentEnergy(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_ENERGYDAMAGE)].percent += percent;
	}
	void setReflectPercentFire(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_FIREDAMAGE)].percent += percent;
	}
	void setReflectPercentPhysical(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_PHYSICALDAMAGE)].percent += percent;
	}
	void setReflectPercentEarth(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_EARTHDAMAGE)].percent += percent;
	}
	void setReflectPercentIce(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_ICEDAMAGE)].percent += percent;
	}
	void setReflectPercentHoly(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_HOLYDAMAGE)].percent += percent;
	}
	void setReflectPercentDeath(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_DEATHDAMAGE)].percent += percent;
	}
	void setReflectPercentLifeDrain(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_LIFEDRAIN)].percent += percent;
	}
	void setReflectPercentManaDrain(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_MANADRAIN)].percent += percent;
	}
	void setReflectPercentDrown(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_DROWNDAMAGE)].percent += percent;
	}
	void setReflectPercentHealing(int16_t percent)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_HEALING)].percent += percent;
	}

	void setReflectChance(int16_t chance)
	{
		for (auto& i : this->abilities->reflect) {
			i.chance += chance;
		}
	}
	void setReflectChanceElements(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_ENERGYDAMAGE)].chance += chance;
		this->abilities->reflect[combatTypeToIndex(COMBAT_FIREDAMAGE)].chance += chance;
		this->abilities->reflect[combatTypeToIndex(COMBAT_EARTHDAMAGE)].chance += chance;
		this->abilities->reflect[combatTypeToIndex(COMBAT_ICEDAMAGE)].chance += chance;
	}
	void setReflectChanceMagic(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_ENERGYDAMAGE)].chance += chance;
		this->abilities->reflect[combatTypeToIndex(COMBAT_FIREDAMAGE)].chance += chance;
		this->abilities->reflect[combatTypeToIndex(COMBAT_EARTHDAMAGE)].chance += chance;
		this->abilities->reflect[combatTypeToIndex(COMBAT_ICEDAMAGE)].chance += chance;
		this->abilities->reflect[combatTypeToIndex(COMBAT_HOLYDAMAGE)].chance += chance;
		this->abilities->reflect[combatTypeToIndex(COMBAT_DEATHDAMAGE)].chance += chance;
	}
	void setReflectChanceEnergy(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_ENERGYDAMAGE)].chance += chance;
	}
	void setReflectChanceFire(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_FIREDAMAGE)].chance += chance;
	}
	void setReflectChancePhysical(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_PHYSICALDAMAGE)].chance += chance;
	}
	void setReflectChanceEarth(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_EARTHDAMAGE)].chance += chance;
	}
	void setReflectChanceIce(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_ICEDAMAGE)].chance += chance;
	}
	void setReflectChanceHoly(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_HOLYDAMAGE)].chance += chance;
	}
	void setReflectChanceDeath(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_DEATHDAMAGE)].chance += chance;
	}
	void setReflectChanceLifeDrain(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_LIFEDRAIN)].chance += chance;
	}
	void setReflectChanceManaDrain(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_MANADRAIN)].chance += chance;
	}
	void setReflectChanceDrown(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_DROWNDAMAGE)].chance += chance;
	}
	void setReflectChanceHealing(int16_t chance)
	{
		this->abilities->reflect[combatTypeToIndex(COMBAT_HEALING)].chance += chance;
	}

	void setBoostPercent(int16_t boost)
	{
		for (auto& i : this->abilities->boostPercent) {
			i += boost;
		}
	}
	void setBoostPercentElements(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += boost;
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += boost;
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += boost;
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_ICEDAMAGE)] += boost;
	}
	void setBoostPercentMagic(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += boost;
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += boost;
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += boost;
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_ICEDAMAGE)] += boost;
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_HOLYDAMAGE)] += boost;
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_DEATHDAMAGE)] += boost;
	}
	void setBoostPercentEnergy(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += boost;
	}
	void setBoostPercentFire(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += boost;
	}
	void setBoostPercentPhysical(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_PHYSICALDAMAGE)] += boost;
	}
	void setBoostPercentEarth(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += boost;
	}
	void setBoostPercentIce(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_ICEDAMAGE)] += boost;
	}
	void setBoostPercentHoly(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_HOLYDAMAGE)] += boost;
	}
	void setBoostPercentDeath(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_DEATHDAMAGE)] += boost;
	}
	void setBoostPercentLifeDrain(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_LIFEDRAIN)] += boost;
	}
	void setBoostPercentManaDrain(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_MANADRAIN)] += boost;
	}
	void setBoostPercentDrown(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_DROWNDAMAGE)] += boost;
	}
	void setBoostPercentHealing(int16_t boost)
	{
		this->abilities->boostPercent[combatTypeToIndex(COMBAT_HEALING)] += boost;
	}

	void setMagicLevelEnergy(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += value;
	}
	void setMagicLevelFire(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_FIREDAMAGE)] += value;
	}
	void setMagicLevelPhysical(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_PHYSICALDAMAGE)] += value;
	}
	void setMagicLevelEarth(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += value;
	}
	void setMagicLevelIce(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_ICEDAMAGE)] += value;
	}
	void setMagicLevelHoly(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_HOLYDAMAGE)] += value;
	}
	void setMagicLevelDeath(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_DEATHDAMAGE)] += value;
	}
	void setMagicLevelLifeDrain(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_LIFEDRAIN)] += value;
	}
	void setMagicLevelManaDrain(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_MANADRAIN)] += value;
	}
	void setMagicLevelDrown(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_DROWNDAMAGE)] += value;
	}
	void setMagicLevelHealing(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_HEALING)] += value;
	}
	void setMagicLevelUndefined(int16_t value)
	{
		this->abilities->specialMagicLevelSkill[combatTypeToIndex(COMBAT_UNDEFINEDDAMAGE)] += value;
	}

	void setSuppressDrunk() { this->abilities->conditionSuppressions |= CONDITION_DRUNK; }
	void setSuppressEnergy() { this->abilities->conditionSuppressions |= CONDITION_ENERGY; }
	void setSuppressFire() { this->abilities->conditionSuppressions |= CONDITION_FIRE; }
	void setSuppressPoison() { this->abilities->conditionSuppressions |= CONDITION_POISON; }
	void setSuppressDrown() { this->abilities->conditionSuppressions |= CONDITION_DROWN; }
	void setSuppressBleeding() { this->abilities->conditionSuppressions |= CONDITION_BLEEDING; }
	void setSuppressFreezing() { this->abilities->conditionSuppressions |= CONDITION_FREEZING; }
	void setSuppressDazzle() { this->abilities->conditionSuppressions |= CONDITION_DAZZLED; }
	void setSuppressCurse() { this->abilities->conditionSuppressions |= CONDITION_CURSED; }

	void setField(const std::string& element, ConditionDamage* cd);

	void setMaleTransformTo(uint16_t id);
	void setFemaleTransformTo(uint16_t id);

	void setIceElementDamage(uint16_t value)
	{
		this->abilities->elementDamage = value;
		this->abilities->elementType = COMBAT_ICEDAMAGE;
	}
	void setFireElementDamage(uint16_t value)
	{
		this->abilities->elementDamage = value;
		this->abilities->elementType = COMBAT_FIREDAMAGE;
	}
	void setEarthElementDamage(uint16_t value)
	{
		this->abilities->elementDamage = value;
		this->abilities->elementType = COMBAT_EARTHDAMAGE;
	}
	void setHolyElementDamage(uint16_t value)
	{
		this->abilities->elementDamage = value;
		this->abilities->elementType = COMBAT_HOLYDAMAGE;
	}
	void setDeathElementDamage(uint16_t value)
	{
		this->abilities->elementDamage = value;
		this->abilities->elementType = COMBAT_DEATHDAMAGE;
	}
	void setEnergyElementDamage(uint16_t value)
	{
		this->abilities->elementDamage = value;
		this->abilities->elementType = COMBAT_ENERGYDAMAGE;
	}

	void setWorth(uint64_t amount)
	{
		if (Items::getInstance().hasCurrencyItemOf(amount)) {
			std::cout << "[Warning - ItemType::setWorth] Duplicated currency worth. Item " << id << " redefines worth "
			          << amount << std::endl;
		} else {
			Items::getInstance().addCurrencyItem(amount, id);
			this->worth = amount;
		}
	}
};

// TODO: move this to Condition when migrated to the new structure
class ConditionDamageBuilder final : virtual XMLElementBuilder<ConditionDamage*>
{
private:
	uint32_t ticks = 0;
	int32_t start = 0;
	int32_t count = 1;

	std::unique_ptr<ConditionDamage> conditionDamage;

public:
	ConditionDamage* loadFromXMLNode(pugi::xml_node node, bool reloading) override;

	ConditionDamageBuilder* withDamage(int32_t damage, int32_t initDamage = -1)
	{
		if (this->start > 0) {
			std::list<int32_t> damageList;
			ConditionDamage::generateDamageList(damage, this->start, damageList);
			for (int32_t damageValue : damageList) {
				this->conditionDamage->addDamage(1, this->ticks, -damageValue);
			}

			this->start = 0;
		} else {
			this->conditionDamage->addDamage(this->count, this->ticks, damage);
		}

		if (initDamage > 0 || initDamage < -1) {
			this->conditionDamage->setInitDamage(-initDamage);
		} else if (initDamage == -1 && this->start != 0) {
			this->conditionDamage->setInitDamage(this->start);
		}

		return this;
	}
};

#endif
