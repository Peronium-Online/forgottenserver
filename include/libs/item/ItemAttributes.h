#ifndef PR_ITEM_ATTRIBUTES_H
#define PR_ITEM_ATTRIBUTES_H

#include "libs/item/ReflectAttribute.h"
#include "libs/item/itemenums.h"
#include "libs/util/datastructures/CustomLuaAttribute.h"

#include <map>
#include <unordered_map>

typedef std::unordered_map<std::string, CustomLuaAttribute> CustomAttributeMap;
typedef union
{
	int64_t integer;
	std::string* string;
	CustomAttributeMap* custom;
} AttributeValue;

class ItemAttributes
{
private:
	struct Attribute
	{
		ItemAttrTypes type;
		AttributeValue value;

		explicit Attribute(ItemAttrTypes type) : type(type) { memset(&value, 0, sizeof(value)); }
		Attribute(const Attribute& i)
		{
			type = i.type;
			if (ItemAttributes::isIntAttrType(type)) {
				value.integer = i.value.integer;
			} else if (ItemAttributes::isStrAttrType(type)) {
				value.string = new std::string(*i.value.string);
			} else if (ItemAttributes::isCustomAttrType(type)) {
				value.custom = new CustomAttributeMap(*i.value.custom);
			} else {
				memset(&value, 0, sizeof(value));
			}
		}
		Attribute(Attribute&& attribute) : value(attribute.value), type(attribute.type)
		{
			memset(&attribute.value, 0, sizeof(value));
			attribute.type = ITEM_ATTRIBUTE_NONE;
		}
		~Attribute()
		{
			if (ItemAttributes::isStrAttrType(type)) {
				delete value.string;
			} else if (ItemAttributes::isCustomAttrType(type)) {
				delete value.custom;
			}
		}

		Attribute& operator=(Attribute other)
		{
			Attribute::swap(*this, other);
			return *this;
		}
		Attribute& operator=(Attribute&& other)
		{
			if (this != &other) {
				if (ItemAttributes::isStrAttrType(type)) {
					delete value.string;
				} else if (ItemAttributes::isCustomAttrType(type)) {
					delete value.custom;
				}

				value = other.value;
				type = other.type;

				memset(&other.value, 0, sizeof(value));
				other.type = ITEM_ATTRIBUTE_NONE;
			}
			return *this;
		}

		static void swap(Attribute& first, Attribute& second)
		{
			std::swap(first.value, second.value);
			std::swap(first.type, second.type);
		}
	};

	std::vector<Attribute> attributes;
	uint32_t attributeBits = 0;

	const static uint32_t intAttributeTypes =
	    ITEM_ATTRIBUTE_ACTIONID | ITEM_ATTRIBUTE_UNIQUEID | ITEM_ATTRIBUTE_DATE | ITEM_ATTRIBUTE_WEIGHT |
	    ITEM_ATTRIBUTE_ATTACK | ITEM_ATTRIBUTE_DEFENSE | ITEM_ATTRIBUTE_EXTRADEFENSE | ITEM_ATTRIBUTE_ARMOR |
	    ITEM_ATTRIBUTE_HITCHANCE | ITEM_ATTRIBUTE_SHOOTRANGE | ITEM_ATTRIBUTE_OWNER | ITEM_ATTRIBUTE_DURATION |
	    ITEM_ATTRIBUTE_DECAYSTATE | ITEM_ATTRIBUTE_CORPSEOWNER | ITEM_ATTRIBUTE_CHARGES | ITEM_ATTRIBUTE_FLUIDTYPE |
	    ITEM_ATTRIBUTE_DOORID | ITEM_ATTRIBUTE_DECAYTO | ITEM_ATTRIBUTE_WRAPID | ITEM_ATTRIBUTE_STOREITEM |
	    ITEM_ATTRIBUTE_ATTACK_SPEED | ITEM_ATTRIBUTE_OPENCONTAINER;
	const static uint32_t stringAttributeTypes = ITEM_ATTRIBUTE_DESCRIPTION | ITEM_ATTRIBUTE_TEXT |
	                                             ITEM_ATTRIBUTE_WRITER | ITEM_ATTRIBUTE_NAME | ITEM_ATTRIBUTE_ARTICLE |
	                                             ITEM_ATTRIBUTE_PLURALNAME;

	const static std::string& EMPTY_STRING;

public:
	ItemAttributes() = default;

	bool equals(const ItemAttributes& other) const;

	static bool isIntAttrType(ItemAttrTypes type) { return (type & intAttributeTypes) == type; }
	static bool isStrAttrType(ItemAttrTypes type) { return (type & stringAttributeTypes) == type; }
	inline static bool isCustomAttrType(ItemAttrTypes type) { return (type & ITEM_ATTRIBUTE_CUSTOM) == type; }

	bool isUndefined() { return attributeBits == 0; }

	const Attribute* getExistingAttr(ItemAttrTypes type) const;

	bool hasAttr(ItemAttrTypes type) const { return (type & attributeBits) != 0; }
	Attribute& getAttr(ItemAttrTypes type);

	void setIntAttr(ItemAttrTypes type, int64_t value);
	int64_t getIntAttr(ItemAttrTypes type) const;
	void increaseIntAttr(ItemAttrTypes type, int64_t value) { setIntAttr(type, getIntAttr(type) + value); }

	void setStrAttr(ItemAttrTypes type, const std::string& value);
	const std::string& getStrAttr(ItemAttrTypes type) const;

	void removeAttr(ItemAttrTypes type);

	CustomAttributeMap* getCustomAttrMap()
	{
		if (!hasAttr(ITEM_ATTRIBUTE_CUSTOM)) {
			return nullptr;
		}

		return getAttr(ITEM_ATTRIBUTE_CUSTOM).value.custom;
	}

	template <typename R>
	void setCustomAttr(int64_t key, R value)
	{
		auto tmp = std::to_string(key);
		setCustomAttr(tmp, value);
	}

	void setCustomAttr(int64_t key, CustomLuaAttribute& value)
	{
		auto tmp = std::to_string(key);
		setCustomAttr(tmp, value);
	}

	template <typename R>
	void setCustomAttr(std::string& key, R value)
	{
		boost::algorithm::to_lower(key);
		if (hasAttr(ITEM_ATTRIBUTE_CUSTOM)) {
			removeCustomAttr(key);
		} else {
			getAttr(ITEM_ATTRIBUTE_CUSTOM).value.custom = new CustomAttributeMap();
		}
		getAttr(ITEM_ATTRIBUTE_CUSTOM).value.custom->emplace(key, value);
	}

	void setCustomAttr(std::string& key, CustomLuaAttribute& value)
	{
		boost::algorithm::to_lower(key);
		if (hasAttr(ITEM_ATTRIBUTE_CUSTOM)) {
			removeCustomAttr(key);
		} else {
			getAttr(ITEM_ATTRIBUTE_CUSTOM).value.custom = new CustomAttributeMap();
		}
		getAttr(ITEM_ATTRIBUTE_CUSTOM).value.custom->insert(std::make_pair(std::move(key), std::move(value)));
	}

	const CustomLuaAttribute* getCustomAttr(int64_t key)
	{
		auto tmp = std::to_string(key);
		return getCustomAttr(tmp);
	}

	const CustomLuaAttribute* getCustomAttr(const std::string& key)
	{
		if (const CustomAttributeMap* customAttrMap = getCustomAttrMap()) {
			auto it = customAttrMap->find(boost::algorithm::to_lower_copy(key));
			if (it != customAttrMap->end()) {
				return &(it->second);
			}
		}
		return nullptr;
	}

	bool removeCustomAttr(int64_t key)
	{
		auto tmp = std::to_string(key);
		return removeCustomAttr(tmp);
	}

	bool removeCustomAttr(const std::string& key)
	{
		if (CustomAttributeMap* customAttrMap = getCustomAttrMap()) {
			auto it = customAttrMap->find(boost::algorithm::to_lower_copy(key));
			if (it != customAttrMap->end()) {
				customAttrMap->erase(it);
				return true;
			}
		}
		return false;
	}

	void forEachAttribute(const std::function<void(ItemAttrTypes type, const AttributeValue attribute)>& func);
};

class MutableItemAttributes
{
private:
	std::map<CombatType_t, Reflect> reflect;
	std::map<CombatType_t, uint16_t> boostPercent;

public:
	MutableItemAttributes() = default;

	void setReflect(CombatType_t combatType, const Reflect& reflect) { this->reflect[combatType] = reflect; }
	const Reflect& getReflect(CombatType_t combatType)
	{
		auto it = reflect.find(combatType);
		return it != reflect.end() ? it->second : Reflect::UNDEFINED;
	}
	std::map<CombatType_t, Reflect> getReflects() { return reflect; }

	void setBoostPercent(CombatType_t combatType, uint16_t percent) { boostPercent[combatType] = percent; }
	int16_t getBoostPercent(CombatType_t combatType)
	{
		auto it = boostPercent.find(combatType);
		return it != boostPercent.end() ? it->second : 0;
	}
	std::map<CombatType_t, uint16_t> getBoosts() { return boostPercent; }
};

#endif
