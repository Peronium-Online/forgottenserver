#ifndef PR_ITEM_H
#define PR_ITEM_H

#include "../../../src/thing.h"
#include "libs/item/ItemAttributes.h"
#include "libs/item/ItemType.h"
#include "libs/item/Items.h"
#include "libs/item/itemenums.h"
#include "libs/util/otb/OTBLoadable.h"

using ItemList = std::list<Item*>;
using ItemDeque = std::deque<Item*>;

struct ItemAttrError : std::runtime_error
{
	ItemAttrTypesIndex attr;
	ItemAttrError(ItemAttrTypesIndex attr, const char* msg) : attr(attr), std::runtime_error(msg) {}

	const char* what() const noexcept override
	{
		std::string errorMsg = "ItemAttrError(" + std::to_string(attr) + "): " + std::runtime_error::what();
		return errorMsg.c_str();
	}
};

class Item : virtual public Thing
{
private:
	uint32_t referenceCounter = 0;
	bool loadedFromMap = false;

	uint16_t count = 1;

	std::string getWeightDescription(uint32_t weight) const;

protected:
	uint16_t id; // the same id as in ItemType

	Cylinder* parent = nullptr;

	const ItemType* iType;
	std::unique_ptr<ItemAttributes> iAttributes;
	std::unique_ptr<MutableItemAttributes> mAttributes;

public:
	Item(const uint16_t type, uint16_t count = 0);
	Item(const Item& i);
	virtual ~Item() = default;

	Item& operator=(const Item&) = delete;
	bool equals(const Item* otherItem) const;
	virtual Item* clone() const;

	static std::string getNameDescription(const ItemType* it, const Item* item = nullptr, int32_t subType = -1,
	                                      bool addArticle = true);
	static std::string getWeightDescription(const ItemType* it, uint32_t weight, uint32_t count = 1);
	std::string getWeightDescription() const;

	virtual void setAttributeFromPropStream(ItemAttrTypesIndex idx, PropStream& stream);

	// TODO: find a better way to handle this methods
	bool hasAttr(ItemAttrTypes attr) { return iAttributes->hasAttr(attr); };
	int64_t getIntAttr(ItemAttrTypes attr) { return iAttributes->getIntAttr(attr); };
	std::string getStringAttr(ItemAttrTypes attr) { return iAttributes->getStrAttr(attr); };
	void setIntAttr(ItemAttrTypes attr, int64_t value) { iAttributes->setIntAttr(attr, value); };
	void setStringAttr(ItemAttrTypes attr, std::string value) { iAttributes->setStrAttr(attr, value); };
	void removeAttr(ItemAttrTypes attr) { iAttributes->removeAttr(attr); };
	const CustomLuaAttribute* getCustomAttr(int64_t key) { return iAttributes->getCustomAttr(key); };
	const CustomLuaAttribute* getCustomAttr(std::string key) { return iAttributes->getCustomAttr(key); };
	void setCustomAttr(std::string key, CustomLuaAttribute value) { iAttributes->setCustomAttr(key, value); };
	bool removeCustomAttr(int64_t key) { return iAttributes->removeCustomAttr(key); };
	bool removeCustomAttr(std::string key) { return iAttributes->removeCustomAttr(key); };
	bool hasProperty(ItemProperties prop) const;

	bool isBlocking() const { return iType->blockSolid; }
	bool isStackable() const { return iType->stackable; }
	bool isAlwaysOnTop() const { return iType->alwaysOnTop; }
	bool isGroundTile() const { return iType->isGroundTile(); }
	bool isMagicField() const { return iType->isMagicField(); }
	bool isMoveable() const { return iType->moveable && !hasUniqueId(); }
	bool isPickupable() const { return iType->pickupable; }
	bool isUseable() const { return iType->useable; }
	bool isHangable() const { return iType->isHangable; }
	bool isRotatable() const
	{
		auto it = iType;

		return it->rotatable && it->rotateTo;
	}
	bool isPodium() const { return iType->isPodium(); }
	bool hasWalkStack() const { return iType->walkStack; }
	bool isSupply() const { return iType->isSupply(); }
	bool isPushable() const override final { return isMoveable(); }
	bool isShowingCount() const { return iType->showCount; }
	bool isFluidContainer() const { return iType->isFluidContainer(); }
	bool isSplash() const { return iType->isSplash(); }
	bool isStoreItem() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_STOREITEM)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_STOREITEM) == 1;
		}
		return iType->storeItem;
	}
	bool isContainer() const { return iType->isContainer(); }
	bool isOpenContainer() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_OPENCONTAINER)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_OPENCONTAINER) > 0;
		}
		return false;
	}
	bool isCleanable() const
	{
		return !loadedFromMap && canRemove() && isPickupable() && !hasUniqueId() &&
		       !iAttributes->hasAttr(ITEM_ATTRIBUTE_ACTIONID);
	}

	bool canReadText() const { return iType->canReadText; }
	bool canWriteText() const { return iType->canWriteText; }
	virtual bool canRemove() const { return true; }
	virtual bool canTransform() const { return true; }

	uint16_t getSubType() const;
	void setSubType(uint16_t n);

	void incrementReferenceCounter() { ++referenceCounter; }
	void decrementReferenceCounter()
	{
		if (--referenceCounter == 0) {
			delete this;
		}
	}

	std::string getDescription(int32_t lookDistance) const override final
	{
		// item descriptions moved to lua
		return "";
	};

	uint16_t getItemCount() const { return count; }
	void setItemCount(uint8_t n) { count = n; }
	uint32_t countByType(int32_t subType)
	{
		if (subType == -1 || subType == getSubType()) {
			return getItemCount();
		}

		return 0;
	}

	bool hasChargesAttribute() const { return iAttributes->hasAttr(ITEM_ATTRIBUTE_CHARGES); }
	uint16_t getCharges() const { return static_cast<uint16_t>(iAttributes->getIntAttr(ITEM_ATTRIBUTE_CHARGES)); }
	void setCharges(uint16_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_CHARGES, n); }

	uint16_t getFluidType() const { return static_cast<uint16_t>(iAttributes->getIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE)); }
	void setFluidType(uint16_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE, n); }

	uint32_t getDuration() const
	{
		if (!iAttributes) {
			return 0;
		}
		return iAttributes->getIntAttr(ITEM_ATTRIBUTE_DURATION);
	}
	void setDuration(int32_t time) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_DURATION, std::max<int32_t>(0, time)); }
	void decreaseDuration(int32_t time) { iAttributes->increaseIntAttr(ITEM_ATTRIBUTE_DURATION, -time); }
	uint32_t getDefaultDuration() const { return iType->decayTime * 1000; }
	void setDefaultDuration()
	{
		uint32_t duration = getDefaultDuration();
		if (duration != 0) {
			setDuration(duration);
		}
	}

	bool canDecay() const;
	ItemDecayState getDecaying() const
	{
		return static_cast<ItemDecayState>(iAttributes->getIntAttr(ITEM_ATTRIBUTE_DECAYSTATE));
	}
	void setDecaying(ItemDecayState decayState) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_DECAYSTATE, decayState); }
	void setDecayTo(int32_t decayTo) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_DECAYTO, decayTo); }
	int32_t getDecayTo() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_DECAYTO)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_DECAYTO);
		}
		return iType->decayTo;
	}
	int32_t getDecayTime() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_DURATION)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_DURATION);
		}
		return iType->decayTime;
	}
	virtual void startDecaying();

	int32_t getThrowRange() const override final { return (isPickupable() ? 15 : 2); }

	virtual uint32_t getWeight() const
	{
		uint32_t weight = getBaseWeight();
		if (isStackable()) {
			return weight * std::max<uint32_t>(1, getItemCount());
		}
		return weight;
	}
	uint32_t getBaseWeight() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_WEIGHT)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_WEIGHT);
		}
		return iType->weight;
	}

	uint16_t getID() const { return id; }
	void setID(uint16_t newid);
	uint16_t getClientID() const { return iType->clientId; }
	uint16_t getActionId() const
	{
		if (!iAttributes) {
			return 0;
		}
		return static_cast<uint16_t>(iAttributes->getIntAttr(ITEM_ATTRIBUTE_ACTIONID));
	}
	void setActionId(uint16_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_ACTIONID, n); }
	bool hasActionId() const { return iAttributes->hasAttr(ITEM_ATTRIBUTE_ACTIONID); }

	bool hasUniqueId() const { return iAttributes->hasAttr(ITEM_ATTRIBUTE_UNIQUEID); }
	uint16_t getUniqueId() const
	{
		if (!iAttributes) {
			return 0;
		}
		return static_cast<uint16_t>(iAttributes->getIntAttr(ITEM_ATTRIBUTE_UNIQUEID));
	}
	void setUniqueId(uint16_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_UNIQUEID, n); }

	const std::string& getText() const { return iAttributes->getStrAttr(ITEM_ATTRIBUTE_TEXT); }
	void setText(const std::string& text) { iAttributes->setStrAttr(ITEM_ATTRIBUTE_TEXT, text); }
	void resetText() { iAttributes->removeAttr(ITEM_ATTRIBUTE_TEXT); }

	time_t getDate() const { return static_cast<time_t>(iAttributes->getIntAttr(ITEM_ATTRIBUTE_DATE)); }
	void setDate(int32_t n) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_DATE, n); }
	void resetDate() { iAttributes->removeAttr(ITEM_ATTRIBUTE_DATE); }

	const std::string& getWriter() const { return iAttributes->getStrAttr(ITEM_ATTRIBUTE_WRITER); }
	void setWriter(const std::string& writer) { iAttributes->setStrAttr(ITEM_ATTRIBUTE_WRITER, writer); }
	void resetWriter() { iAttributes->removeAttr(ITEM_ATTRIBUTE_WRITER); }

	const std::string& getSpecialDescription() const { return iAttributes->getStrAttr(ITEM_ATTRIBUTE_DESCRIPTION); }
	void setSpecialDescription(const std::string& desc) { iAttributes->setStrAttr(ITEM_ATTRIBUTE_DESCRIPTION, desc); }

	ItemTypes getType() const { return iType->type; }

	uint32_t getCorpseOwner() const { return iAttributes->getIntAttr(ITEM_ATTRIBUTE_CORPSEOWNER); }

	uint16_t getMaxTextLen() const { return iType->maxTextLen; }

	int32_t getAttack() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_ATTACK)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_ATTACK);
		}
		return iType->attack;
	}

	uint32_t getAttackSpeed() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_ATTACK_SPEED)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_ATTACK_SPEED);
		}
		return iType->attackSpeed;
	}
	void setAttackSpeed(uint32_t speed)
	{
		if (speed < 100) {
			speed = 100;
		}

		this->iAttributes->setIntAttr(ITEM_ATTRIBUTE_ATTACK_SPEED, speed);
	}

	int32_t getDefense() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_DEFENSE)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_DEFENSE);
		}
		return iType->defense;
	}

	int32_t getExtraDefense() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_EXTRADEFENSE)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_EXTRADEFENSE);
		}
		return iType->extraDefense;
	}

	int32_t getArmor() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_ARMOR)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_ARMOR);
		}
		return iType->armor;
	}

	std::string getNameDescription() const;

	const std::string& getName() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_NAME)) {
			return iAttributes->getStrAttr(ITEM_ATTRIBUTE_NAME);
		}
		return iType->name;
	}
	const std::string getPluralName() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_PLURALNAME)) {
			return iAttributes->getStrAttr(ITEM_ATTRIBUTE_PLURALNAME);
		}
		return iType->getPluralName();
	}

	Cylinder* getTopParent();
	const Cylinder* getTopParent() const;

	const Player* getHoldingPlayer() const;

	const std::string getArticle() const { return iAttributes->getStrAttr(ITEM_ATTRIBUTE_ARTICLE); }

	int8_t getHitChance() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_HITCHANCE)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_HITCHANCE);
		}
		return iType->hitChance;
	}

	uint8_t getShootRange() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_SHOOTRANGE)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_SHOOTRANGE);
		}
		return iType->shootRange;
	}

	int32_t getWrapId() const
	{
		if (iAttributes->hasAttr(ITEM_ATTRIBUTE_WRAPID)) {
			return iAttributes->getIntAttr(ITEM_ATTRIBUTE_WRAPID);
		}
		return 0;
	}
	bool hasWrapId() const { return iAttributes->hasAttr(ITEM_ATTRIBUTE_WRAPID); }

	uint32_t getOwner() const { return iAttributes->getIntAttr(ITEM_ATTRIBUTE_OWNER); }
	void setOwner(uint32_t owner) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_OWNER, owner); }

	WeaponTypes getWeaponType() const { return iType->weaponType; }

	AmmoTypes getAmmoType() const { return iType->ammoType; }

	bool hasCustomAttribute() const { return iAttributes->hasAttr(ITEM_ATTRIBUTE_CUSTOM); }
	std::unordered_map<std::string, CustomLuaAttribute>* getCustomAttributeMap() const
	{
		return iAttributes->getCustomAttrMap();
	}

	std::map<CombatType_t, Reflect> getReflects() const { return mAttributes->getReflects(); }
	const Reflect& getReflect(CombatType_t combatType) { return mAttributes->getReflect(combatType); }
	Reflect getReflect(CombatType_t combatType, bool total = true) const;
	void setReflect(CombatType_t combatType, Reflect reflect) { mAttributes->setReflect(combatType, reflect); }

	std::map<CombatType_t, uint16_t> getBoosts() const { return mAttributes->getBoosts(); }
	int16_t getBoostPercent(CombatType_t combatType) { return mAttributes->getBoostPercent(combatType); }
	uint16_t getBoostPercent(CombatType_t combatType, bool total = true) const;
	void setBoostPercent(CombatType_t combatType, uint16_t boostPercent)
	{
		mAttributes->setBoostPercent(combatType, boostPercent);
	}

	std::array<int16_t, COMBAT_COUNT> getAbsorbs() const { return iType->abilities->absorbPercent; }
	std::array<int16_t, COMBAT_COUNT> getFieldAbsorbs() const { return iType->abilities->fieldAbsorbPercent; }

	uint8_t getOpenContainerCID() const { return iAttributes->getIntAttr(ITEM_ATTRIBUTE_OPENCONTAINER); }
	void setOpenContainerCID(uint8_t cid) { iAttributes->setIntAttr(ITEM_ATTRIBUTE_OPENCONTAINER, cid); }

	int32_t getSlotPosition() const { return iType->slotPosition; }

	uint32_t getWorth() const;

	LightInfo getLightInfo() const;

	virtual void onRemoved();
	virtual void onTradeEvent(TradeEvents, Player*) {}

	bool isLoadedFromMap() const { return loadedFromMap; }
	void setLoadedFromMap(bool value) { loadedFromMap = value; }

	void setDefaultSubtype();

	bool hasMarketAttributes() const;

	bool hasUsedAttributes() const;

	void setStoreItem(bool storeItem)
	{
		iAttributes->setIntAttr(ITEM_ATTRIBUTE_STOREITEM, static_cast<int64_t>(storeItem));
	}

	bool hasAbilities() const { return !!iType->abilities; }
	uint8_t getClassification() const { return iType->classification; }
	uint8_t getTransformEquipTo() const { return iType->transformEquipTo; }
	bool hasInvisibleAbility() const { return iType->abilities->invisible; };
	bool hasManaShieldAbility() const { return iType->abilities->manaShield; };
	int32_t getSkill(uint8_t skill) const { return iType->abilities->skills[skill]; };
	int32_t getSpecialSkill(uint8_t skill) const { return iType->abilities->specialSkills[skill]; };
	int32_t getSpecialMagicLevelSkill(uint8_t skill) const { return iType->abilities->specialMagicLevelSkill[skill]; };
	int32_t getStats(uint8_t stat) const { return iType->abilities->stats[stat]; };
	int32_t getStatPercent(uint8_t stat) const { return iType->abilities->statsPercent[stat]; };
	bool hasRegenerationAbility() const { return iType->abilities->regeneration; };
	uint32_t getHealthGain() const { return iType->abilities->healthGain; };
	uint32_t getHealthTicks() const { return iType->abilities->healthTicks; };
	uint32_t getManaGain() const { return iType->abilities->manaGain; };
	uint32_t getManaTicks() const { return iType->abilities->manaTicks; };
	int32_t getSpeedIncrease() const { return iType->abilities->speed; };
	uint32_t getConditionSuppressions() const { return iType->abilities->conditionSuppressions; };
};

#endif
