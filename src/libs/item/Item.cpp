#include "libs/item/Item.h"

#include "../../container.h"
#include "../../game.h"
#include "libs/item/ItemFactory.h"
#include "libs/item/Items.h"

#include <sstream>

extern Game g_game;

Item::Item(const uint16_t type, uint16_t count) : id(type)
{
	const ItemType* it = Items::getInstance().getItemType(id);
	this->iType = it;

	this->iAttributes = std::make_unique<ItemAttributes>();
	this->mAttributes = std::make_unique<MutableItemAttributes>();

	if (it->isFluidContainer() || it->isSplash()) {
		setFluidType(count);
	} else if (it->stackable) {
		if (count != 0) {
			setItemCount(count);
		} else if (it->charges != 0) {
			setItemCount(it->charges);
		}
	} else if (it->charges != 0) {
		if (count != 0) {
			setCharges(count);
		} else {
			setCharges(it->charges);
		}
	}

	setDefaultDuration();
}

Item::Item(const Item& i) : Thing(), id(i.id), count(i.count), loadedFromMap(i.loadedFromMap), iType(i.iType)
{
	if (i.iAttributes) {
		iAttributes.reset(new ItemAttributes(*i.iAttributes));
	}

	if (i.mAttributes) {
		mAttributes.reset(new MutableItemAttributes(*i.mAttributes));
	}
}

Item* Item::clone() const
{
	Item* item = ItemFactory::create(id, count);
	if (iAttributes) {
		item->iAttributes.reset(new ItemAttributes(*iAttributes));
		item->mAttributes.reset(new MutableItemAttributes(*mAttributes));
		if (item->getDuration() > 0) {
			item->incrementReferenceCounter();
			item->setDecaying(DECAYING_TRUE);
			g_game.toDecayItems.push_front(item);
		}
	}
	return item;
}

bool Item::equals(const Item* otherItem) const
{
	if (!otherItem || id != otherItem->id) {
		return false;
	}

	const auto& otherAttributes = otherItem->iAttributes;
	if (!iAttributes) {
		return !otherAttributes || otherAttributes->isUndefined();
	} else if (!otherAttributes) {
		return (iAttributes->isUndefined());
	}

	return otherAttributes->equals(*iAttributes);
}

void Item::setSubType(uint16_t n)
{
	if (iType->isFluidContainer() || iType->isSplash()) {
		setFluidType(n);
	} else if (iType->stackable) {
		setItemCount(n);
	} else if (iType->charges != 0) {
		setCharges(n);
	} else {
		setItemCount(n);
	}
}

uint16_t Item::getSubType() const
{
	if (iType->isFluidContainer() || iType->isSplash()) {
		return getFluidType();
	}

	if (iType->stackable) {
		return getItemCount();
	}

	if (iType->charges != 0) {
		return getCharges();
	}

	return getItemCount();
}

void Item::setAttributeFromPropStream(ItemAttrTypesIndex idx, PropStream& stream)
{
	switch (idx) {
		case ATTR_COUNT:
		case ATTR_RUNE_CHARGES: {
			uint8_t count;
			if (!stream.read<uint8_t>(count)) {
				throw ItemAttrError{ATTR_COUNT, "Unable to read count attribute"};
			}

			setSubType(count);
			break;
		}

		case ATTR_ACTION_ID: {
			uint16_t actionId;
			if (!stream.read<uint16_t>(actionId)) {
				throw ItemAttrError{ATTR_ACTION_ID, "Unable to read actionId attribute"};
			}

			setActionId(actionId);
			break;
		}

		case ATTR_UNIQUE_ID: {
			uint16_t uniqueId;
			if (!stream.read<uint16_t>(uniqueId)) {
				throw ItemAttrError{ATTR_UNIQUE_ID, "Unable to read uniqueId attribute"};
			}

			setUniqueId(uniqueId);
			break;
		}

		case ATTR_TEXT: {
			std::string text;
			if (!stream.readString(text)) {
				throw ItemAttrError{ATTR_TEXT, "Unable to read text attribute"};
			}

			setText(text);
			break;
		}

		case ATTR_WRITTENDATE: {
			uint32_t writtenDate;
			if (!stream.read<uint32_t>(writtenDate)) {
				throw ItemAttrError{ATTR_WRITTENDATE, "Unable to read writtenDate attribute"};
			}

			setDate(writtenDate);
			break;
		}

		case ATTR_WRITTENBY: {
			std::string writer;
			if (!stream.readString(writer)) {
				throw ItemAttrError{ATTR_WRITTENBY, "Unable to read writer attribute"};
			}

			setWriter(writer);
			break;
		}

		case ATTR_DESC: {
			std::string text;
			if (!stream.readString(text)) {
				throw ItemAttrError{ATTR_DESC, "Unable to read description attribute"};
			}

			setSpecialDescription(text);
			break;
		}

		case ATTR_CHARGES: {
			uint16_t charges;
			if (!stream.read<uint16_t>(charges)) {
				throw ItemAttrError{ATTR_CHARGES, "Unable to read charges attribute"};
			}

			setSubType(charges);
			break;
		}

		case ATTR_DURATION: {
			int32_t duration;
			if (!stream.read<int32_t>(duration)) {
				throw ItemAttrError{ATTR_DURATION, "Unable to read duration attribute"};
			}

			setDuration(duration);
			break;
		}

		case ATTR_DECAYING_STATE: {
			uint8_t state;
			if (!stream.read<uint8_t>(state)) {
				throw ItemAttrError{ATTR_DECAYING_STATE, "Unable to read decayState attribute"};
			}

			if (state != DECAYING_FALSE) {
				setDecaying(DECAYING_PENDING);
			}
			break;
		}

		case ATTR_NAME: {
			std::string name;
			if (!stream.readString(name)) {
				throw ItemAttrError{ATTR_NAME, "Unable to read name attribute"};
			}

			iAttributes->setStrAttr(ITEM_ATTRIBUTE_NAME, name);
			break;
		}

		case ATTR_ARTICLE: {
			std::string article;
			if (!stream.readString(article)) {
				throw ItemAttrError{ATTR_ARTICLE, "Unable to read article attribute"};
			}

			iAttributes->setStrAttr(ITEM_ATTRIBUTE_ARTICLE, article);
			break;
		}

		case ATTR_PLURALNAME: {
			std::string pluralName;
			if (!stream.readString(pluralName)) {
				throw ItemAttrError{ATTR_PLURALNAME, "Unable to read pluralName attribute"};
			}

			iAttributes->setStrAttr(ITEM_ATTRIBUTE_PLURALNAME, pluralName);
			break;
		}

		case ATTR_WEIGHT: {
			uint32_t weight;
			if (!stream.read<uint32_t>(weight)) {
				throw ItemAttrError{ATTR_WEIGHT, "Unable to read weight attribute"};
			}

			iAttributes->setIntAttr(ITEM_ATTRIBUTE_WEIGHT, weight);
			break;
		}

		case ATTR_ATTACK: {
			int32_t attack;
			if (!stream.read<int32_t>(attack)) {
				throw ItemAttrError{ATTR_ATTACK, "Unable to read attack attribute"};
			}

			iAttributes->setIntAttr(ITEM_ATTRIBUTE_ATTACK, attack);
			break;
		}

		case ATTR_ATTACK_SPEED: {
			uint32_t attackSpeed;
			if (!stream.read<uint32_t>(attackSpeed)) {
				throw ItemAttrError{ATTR_ATTACK_SPEED, "Unable to read attackSpeed attribute"};
			}

			this->setAttackSpeed(attackSpeed);
			break;
		}

		case ATTR_DEFENSE: {
			int32_t defense;
			if (!stream.read<int32_t>(defense)) {
				throw ItemAttrError{ATTR_DEFENSE, "Unable to read defense attribute"};
			}

			iAttributes->setIntAttr(ITEM_ATTRIBUTE_DEFENSE, defense);
			break;
		}

		case ATTR_EXTRADEFENSE: {
			int32_t extraDefense;
			if (!stream.read<int32_t>(extraDefense)) {
				throw ItemAttrError{ATTR_EXTRADEFENSE, "Unable to read extraDefense attribute"};
			}

			iAttributes->setIntAttr(ITEM_ATTRIBUTE_EXTRADEFENSE, extraDefense);
			break;
		}

		case ATTR_ARMOR: {
			int32_t armor;
			if (!stream.read<int32_t>(armor)) {
				throw ItemAttrError{ATTR_ARMOR, "Unable to read armor attribute"};
			}

			iAttributes->setIntAttr(ITEM_ATTRIBUTE_ARMOR, armor);
			break;
		}

		case ATTR_HITCHANCE: {
			int8_t hitChance;
			if (!stream.read<int8_t>(hitChance)) {
				throw ItemAttrError{ATTR_HITCHANCE, "Unable to read hitChance attribute"};
			}

			iAttributes->setIntAttr(ITEM_ATTRIBUTE_HITCHANCE, hitChance);
			break;
		}

		case ATTR_SHOOTRANGE: {
			uint8_t shootRange;
			if (!stream.read<uint8_t>(shootRange)) {
				throw ItemAttrError{ATTR_SHOOTRANGE, "Unable to read shootRange attribute"};
			}

			iAttributes->setIntAttr(ITEM_ATTRIBUTE_SHOOTRANGE, shootRange);
			break;
		}

		case ATTR_DECAYTO: {
			int32_t decayTo;
			if (!stream.read<int32_t>(decayTo)) {
				throw ItemAttrError{ATTR_DECAYTO, "Unable to read decayTo attribute"};
			}

			iAttributes->setIntAttr(ITEM_ATTRIBUTE_DECAYTO, decayTo);
			break;
		}

		case ATTR_WRAPID: {
			uint16_t wrapId;
			if (!stream.read<uint16_t>(wrapId)) {
				throw ItemAttrError{ATTR_WRAPID, "Unable to read wrapId attribute"};
			}

			iAttributes->setIntAttr(ITEM_ATTRIBUTE_WRAPID, wrapId);
			break;
		}

		case ATTR_STOREITEM: {
			uint8_t storeItem;
			if (!stream.read<uint8_t>(storeItem)) {
				throw ItemAttrError{ATTR_STOREITEM, "Unable to read storeItem attribute"};
			}

			iAttributes->setIntAttr(ITEM_ATTRIBUTE_STOREITEM, storeItem);
			break;
		}

		case ATTR_OPENCONTAINER: {
			uint8_t openContainer;
			if (!stream.read<uint8_t>(openContainer)) {
				throw ItemAttrError{ATTR_OPENCONTAINER, "Unable to read openContainer attribute"};
			}

			iAttributes->setIntAttr(ITEM_ATTRIBUTE_OPENCONTAINER, openContainer);
			break;
		}

		case ATTR_REFLECT: {
			uint16_t size;
			if (!stream.read<uint16_t>(size)) {
				throw ItemAttrError{ATTR_REFLECT, "Unable to read reflect attribute"};
			}

			for (uint16_t i = 0; i < size; ++i) {
				CombatType_t combatType;
				Reflect reflect;

				if (!stream.read<CombatType_t>(combatType) || !stream.read<uint16_t>(reflect.percent) ||
				    !stream.read<uint16_t>(reflect.chance)) {
					throw ItemAttrError{ATTR_REFLECT, "Unable to read reflect attribute"};
				}

				mAttributes->setReflect(combatType, reflect);
			}
			break;
		}

		case ATTR_BOOST: {
			uint16_t size;
			if (!stream.read<uint16_t>(size)) {
				throw ItemAttrError{ATTR_BOOST, "Unable to read boost attribute"};
			}

			for (uint16_t i = 0; i < size; ++i) {
				CombatType_t combatType;
				uint16_t percent;

				if (!stream.read<CombatType_t>(combatType) || !stream.read<uint16_t>(percent)) {
					throw ItemAttrError{ATTR_BOOST, "Unable to read boost attribute"};
				}

				mAttributes->setBoostPercent(combatType, percent);
			}
			break;
		}

		// these should be handled through derived classes If these are called then something has changed in the
		// items.xml since the map was saved just read the values

		// Depot class
		case ATTR_DEPOT_ID: {
			if (!stream.skip(2)) {
				throw ItemAttrError{ATTR_DEPOT_ID, "Unable to read depotId attribute"};
			}
			break;
		}

		// Door class
		case ATTR_HOUSEDOORID: {
			if (!stream.skip(1)) {
				throw ItemAttrError{ATTR_HOUSEDOORID, "Unable to read houseDoorId attribute"};
			}
			break;
		}

		// Bed class
		case ATTR_SLEEPERGUID: {
			if (!stream.skip(4)) {
				throw ItemAttrError{ATTR_SLEEPERGUID, "Unable to read sleeperGuid attribute"};
			}
			break;
		}

		case ATTR_SLEEPSTART: {
			if (!stream.skip(4)) {
				throw ItemAttrError{ATTR_SLEEPSTART, "Unable to read sleepStart attribute"};
			}
			break;
		}

		// Podium class
		case ATTR_PODIUMOUTFIT: {
			if (!stream.skip(15)) {
				throw ItemAttrError{ATTR_PODIUMOUTFIT, "Unable to read podiumOutfit attribute"};
			}
			break;
		}

		// Teleport class
		case ATTR_TELE_DEST: {
			if (!stream.skip(5)) {
				throw ItemAttrError{ATTR_TELE_DEST, "Unable to read teleDestattribute"};
			}
			break;
		}

		// Container class
		case ATTR_CONTAINER_ITEMS: {
			throw ItemAttrError{ATTR_CONTAINER_ITEMS, "Unable to containerItems attribute"};
		}

		case ATTR_CUSTOM_ATTRIBUTES: {
			uint64_t size;
			if (!stream.read<uint64_t>(size)) {
				throw ItemAttrError{ATTR_CUSTOM_ATTRIBUTES, "Unable to read customAttributes attribute"};
			}

			for (uint64_t i = 0; i < size; i++) {
				// Unserialize key type and value
				std::string key;
				if (!stream.readString(key)) {
					throw ItemAttrError{ATTR_CUSTOM_ATTRIBUTES, "Unable to read customAttributes key attribute"};
				};

				// Unserialize value type and value
				CustomLuaAttribute val;
				if (!val.unserialize(stream)) {
					throw ItemAttrError{ATTR_CUSTOM_ATTRIBUTES, "Unable to read customAttributes val attribute"};
				}

				iAttributes->setCustomAttr(key, val);
			}
			break;
		}

		default:
			throw ItemAttrError{ATTR_UNKNOWN, "Unknown attribute"};
	}
}

Cylinder* Item::getTopParent()
{
	Cylinder* aux = getParent();
	Cylinder* prevaux = dynamic_cast<Cylinder*>(this);
	if (!aux) {
		return prevaux;
	}

	while (aux->getParent()) {
		prevaux = aux;
		aux = aux->getParent();
	}

	if (prevaux) {
		return prevaux;
	}
	return aux;
}

const Cylinder* Item::getTopParent() const
{
	const Cylinder* aux = getParent();
	const Cylinder* prevaux = dynamic_cast<const Cylinder*>(this);
	if (!aux) {
		return prevaux;
	}

	while (aux->getParent()) {
		prevaux = aux;
		aux = aux->getParent();
	}

	if (prevaux) {
		return prevaux;
	}
	return aux;
}

void Item::onRemoved()
{
	ScriptEnvironment::removeTempItem(this);

	if (iAttributes->hasAttr(ITEM_ATTRIBUTE_UNIQUEID)) {
		g_game.removeUniqueItem(getUniqueId());
	}
}

bool Item::canDecay() const
{
	if (isRemoved()) {
		return false;
	}

	if (getDecayTo() < 0 || getDecayTime() == 0) {
		return false;
	}

	if (hasUniqueId()) {
		return false;
	}

	return true;
}

void Item::startDecaying() { g_game.startDecay(this); }

void Item::setID(uint16_t newid)
{
	id = newid;

	auto newIType = Items::getInstance().getItemType(newid);
	uint32_t newDuration = newIType->decayTime * 1000;

	if (newDuration == 0 && !newIType->stopTime && newIType->decayTo < 0) {
		iAttributes->removeAttr(ITEM_ATTRIBUTE_DECAYSTATE);
		iAttributes->removeAttr(ITEM_ATTRIBUTE_DURATION);
	}

	iAttributes->removeAttr(ITEM_ATTRIBUTE_CORPSEOWNER);

	if (newDuration > 0 && (!iType->stopTime || !iAttributes->hasAttr(ITEM_ATTRIBUTE_DURATION))) {
		setDecaying(DECAYING_FALSE);
		setDuration(newDuration);
	}

	iType = newIType;
}

const Player* Item::getHoldingPlayer() const { return dynamic_cast<const Player*>(getTopParent()); }

uint32_t Item::getWorth() const { return iType->worth * count; }

LightInfo Item::getLightInfo() const { return {iType->lightLevel, iType->lightColor}; }

std::string Item::getNameDescription(const ItemType* it, const Item* item /*= nullptr*/, int32_t subType /*= -1*/,
                                     bool addArticle /*= true*/)
{
	if (item) {
		subType = item->getSubType();
	}

	std::ostringstream s;

	const std::string& name = (item ? item->getName() : it->name);
	if (!name.empty()) {
		if (it->stackable && subType > 1) {
			if (it->showCount) {
				s << subType << ' ';
			}

			s << (item ? item->getPluralName() : it->getPluralName());
		} else {
			if (addArticle) {
				const std::string& article = (item ? item->getArticle() : it->article);
				if (!article.empty()) {
					s << article << ' ';
				}
			}

			s << name;
		}
	} else {
		if (addArticle) {
			s << "an ";
		}
		s << "item of type " << it->id;
	}
	return s.str();
}

std::string Item::getNameDescription() const { return getNameDescription(iType, this); }

std::string Item::getWeightDescription(const ItemType* it, uint32_t weight, uint32_t count /*= 1*/)
{
	std::ostringstream ss;
	if (it->stackable && count > 1 && it->showCount != 0) {
		ss << "They weigh ";
	} else {
		ss << "It weighs ";
	}

	if (weight < 10) {
		ss << "0.0" << weight;
	} else if (weight < 100) {
		ss << "0." << weight;
	} else {
		std::string weightString = std::to_string(weight);
		weightString.insert(weightString.end() - 2, '.');
		ss << weightString;
	}

	ss << " oz.";
	return ss.str();
}

std::string Item::getWeightDescription(uint32_t weight) const
{
	return getWeightDescription(iType, weight, getItemCount());
}

std::string Item::getWeightDescription() const
{
	uint32_t weight = getWeight();
	if (weight == 0) {
		return std::string();
	}
	return getWeightDescription(weight);
}

void Item::setDefaultSubtype()
{
	setItemCount(1);

	if (iType->charges != 0) {
		if (iType->stackable) {
			setItemCount(iType->charges);
		} else {
			setCharges(iType->charges);
		}
	}
}

Reflect Item::getReflect(CombatType_t combatType, bool total /* = true */) const
{
	Reflect reflect;
	if (mAttributes) {
		reflect += mAttributes->getReflect(combatType);
	}

	if (total && iType->abilities) {
		reflect += iType->abilities->reflect[combatTypeToIndex(combatType)];
	}

	return reflect;
}

uint16_t Item::getBoostPercent(CombatType_t combatType, bool total /* = true */) const
{
	uint16_t boostPercent = 0;
	if (mAttributes) {
		boostPercent += mAttributes->getBoostPercent(combatType);
	}

	if (total && iType->abilities) {
		boostPercent += iType->abilities->boostPercent[combatTypeToIndex(combatType)];
	}

	return boostPercent;
}

bool Item::hasUsedAttributes() const
{
	bool has = false;
	iAttributes->forEachAttribute([&](ItemAttrTypes type, const AttributeValue val) {
		if (type == ITEM_ATTRIBUTE_CHARGES) {
			uint16_t charges = static_cast<uint16_t>(val.integer);
			if (charges != iType->charges) {
				has = true;
			}
		} else if (type == ITEM_ATTRIBUTE_DURATION) {
			uint32_t duration = static_cast<uint32_t>(val.integer);
			if (duration != getDefaultDuration()) {
				has = true;
			}
		}
		// this line below was in the original version, but it seems to be introducing a bug
		// else {
		// 	return true;
		// }
	});

	return has;
}

bool Item::hasMarketAttributes() const
{
	if (!iAttributes) {
		return true;
	}

	// discard items with custom boost and reflect
	for (uint16_t i = 0; i < COMBAT_COUNT; ++i) {
		if (getBoostPercent(indexToCombatType(i), false) > 0) {
			return false;
		}

		Reflect tmpReflect = getReflect(indexToCombatType(i), false);
		if (tmpReflect.chance != 0 || tmpReflect.percent != 0) {
			return false;
		}
	}

	// discard items with other modified attributes
	return !hasUsedAttributes();
}

bool Item::hasProperty(ItemProperties prop) const
{
	switch (prop) {
		case CONST_PROP_BLOCKSOLID:
			return iType->blockSolid;
		case CONST_PROP_MOVEABLE:
			return iType->moveable && !hasUniqueId();
		case CONST_PROP_HASHEIGHT:
			return iType->hasHeight;
		case CONST_PROP_BLOCKPROJECTILE:
			return iType->blockProjectile;
		case CONST_PROP_BLOCKPATH:
			return iType->blockPathFind;
		case CONST_PROP_ISVERTICAL:
			return iType->isVertical;
		case CONST_PROP_ISHORIZONTAL:
			return iType->isHorizontal;
		case CONST_PROP_IMMOVABLEBLOCKSOLID:
			return iType->blockSolid && (!iType->moveable || hasUniqueId());
		case CONST_PROP_IMMOVABLEBLOCKPATH:
			return iType->blockPathFind && (!iType->moveable || hasUniqueId());
		case CONST_PROP_IMMOVABLENOFIELDBLOCKPATH:
			return !iType->isMagicField() && iType->blockPathFind && (!iType->moveable || hasUniqueId());
		case CONST_PROP_NOFIELDBLOCKPATH:
			return !iType->isMagicField() && iType->blockPathFind;
		case CONST_PROP_SUPPORTHANGABLE:
			return iType->isHorizontal || iType->isVertical;
		default:
			return false;
	}
}
