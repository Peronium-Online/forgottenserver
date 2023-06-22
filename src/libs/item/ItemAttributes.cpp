#include "libs/item/ItemAttributes.h"

#include <iostream>

bool ItemAttributes::equals(const ItemAttributes& other) const
{
	if (attributeBits != other.attributeBits) {
		return false;
	}

	const auto& otherAttributeList = other.attributes;
	for (const auto& attribute : attributes) {
		if (ItemAttributes::isIntAttrType(attribute.type)) {
			for (const auto& otherAttribute : otherAttributeList) {
				if (attribute.type == otherAttribute.type && attribute.value.integer != otherAttribute.value.integer) {
					return false;
				}
			}
		} else if (ItemAttributes::isStrAttrType(attribute.type)) {
			for (const auto& otherAttribute : otherAttributeList) {
				if (attribute.type == otherAttribute.type && *attribute.value.string != *otherAttribute.value.string) {
					return false;
				}
			}
		} else {
			for (const auto& otherAttribute : otherAttributeList) {
				if (attribute.type == otherAttribute.type && *attribute.value.custom != *otherAttribute.value.custom) {
					return false;
				}
			}
		}
	}
	return true;
}

const ItemAttributes::Attribute* ItemAttributes::getExistingAttr(ItemAttrTypes type) const
{
	if (hasAttribute(type)) {
		for (const Attribute& attribute : attributes) {
			if (attribute.type == type) {
				return &attribute;
			}
		}
	}
	return nullptr;
}

ItemAttributes::Attribute& ItemAttributes::getAttr(ItemAttrTypes type)
{
	if (hasAttribute(type)) {
		for (Attribute& attribute : attributes) {
			if (attribute.type == type) {
				return attribute;
			}
		}
	}

	attributeBits |= type;
	attributes.emplace_back(type);
	return attributes.back();
}

int64_t ItemAttributes::getIntAttr(ItemAttrTypes type) const
{
	if (!isIntAttrType(type)) {
		return 0;
	}

	const Attribute* attr = getExistingAttr(type);
	if (!attr) {
		return 0;
	}
	return attr->value.integer;
}

void ItemAttributes::setIntAttr(ItemAttrTypes type, int64_t value)
{
	if (!isIntAttrType(type)) {
		return;
	}

	if (type == ITEM_ATTRIBUTE_ATTACK_SPEED && value < 100) {
		value = 100;
	}

	getAttr(type).value.integer = value;
}

const std::string& ItemAttributes::getStrAttr(ItemAttrTypes type) const
{
	if (!isStrAttrType(type)) {
		std::cout << "Warning: trying to get non-string attribute: " << type << std::endl;

		return "";
	}

	const Attribute* attr = getExistingAttr(type);
	if (!attr) {
		return "";
	}
	return *attr->value.string;
}

void ItemAttributes::setStrAttr(ItemAttrTypes type, const std::string& value)
{
	if (!isStrAttrType(type)) {
		return;
	}

	if (value.empty()) {
		return;
	}

	Attribute& attr = getAttr(type);
	delete attr.value.string;
	attr.value.string = new std::string(value);
}
