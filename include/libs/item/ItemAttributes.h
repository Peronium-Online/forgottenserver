#ifndef PR_ITEM_ATTRIBUTES_H
#define PR_ITEM_ATTRIBUTES_H

#include "libs/item/itemenums.h"
#include "libs/util/datastructures/CustomLuaAttribute.h"

#include <unordered_map>

class ItemAttributes
{
private:
	struct Attribute
	{
		ItemAttrTypes type;
		union
		{
			int64_t integer;
			std::string* string;
			std::unordered_map<std::string, CustomLuaAttribute>* custom;
		} value;

		explicit Attribute(ItemAttrTypes type) : type(type) { memset(&value, 0, sizeof(value)); }
		Attribute(const Attribute& i)
		{
			type = i.type;
			if (ItemAttributes::isIntAttrType(type)) {
				value.integer = i.value.integer;
			} else if (ItemAttributes::isStrAttrType(type)) {
				value.string = new std::string(*i.value.string);
			} else if (ItemAttributes::isCustomAttrType(type)) {
				value.custom = new std::unordered_map<std::string, CustomLuaAttribute>(*i.value.custom);
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

public:
	ItemAttributes() = default;

	static bool isIntAttrType(ItemAttrTypes type) { return (type & intAttributeTypes) == type; }
	static bool isStrAttrType(ItemAttrTypes type) { return (type & stringAttributeTypes) == type; }
	inline static bool isCustomAttrType(ItemAttrTypes type) { return (type & ITEM_ATTRIBUTE_CUSTOM) == type; }

	const Attribute* getExistingAttr(ItemAttrTypes type) const;

	bool hasAttribute(ItemAttrTypes type) const { return (type & attributeBits) != 0; }
	Attribute& getAttr(ItemAttrTypes type);

	void setIntAttr(ItemAttrTypes type, int64_t value);
	int64_t getIntAttr(ItemAttrTypes type) const;
};

#endif
