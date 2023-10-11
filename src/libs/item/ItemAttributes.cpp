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
	if (hasAttr(type)) {
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
	if (hasAttr(type)) {
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

void ItemAttributes::removeAttr(ItemAttrTypes type)
{
	if (!hasAttr(type)) {
		return;
	}

	auto prev_it = attributes.rbegin();
	if ((*prev_it).type == type) {
		attributes.pop_back();
	} else {
		auto it = prev_it, end = attributes.rend();
		while (++it != end) {
			if ((*it).type == type) {
				(*it) = attributes.back();
				attributes.pop_back();
				break;
			}
		}
	}
	attributeBits &= ~type;
}

void ItemAttributes::forEachAttribute(const std::function<void(ItemAttrTypes type, const AttributeValue value)>& func)
{
	for (const Attribute& attribute : attributes) {
		func(attribute.type, attribute.value);
	}
}
