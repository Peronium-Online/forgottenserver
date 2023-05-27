#ifndef PR_OUTFIT_H
#define PR_OUTFIT_H

#include <cinttypes>
#include <string>

struct Outfit
{
	Outfit(std::string name, uint16_t lookType, bool premium, bool unlocked) :
	    name(std::move(name)), lookType(lookType), premium(premium), unlocked(unlocked)
	{}

	bool operator==(const Outfit& otherOutfit) const
	{
		return name == otherOutfit.name && lookType == otherOutfit.lookType && premium == otherOutfit.premium &&
		       unlocked == otherOutfit.unlocked;
	}

	std::string name;
	uint16_t lookType;
	bool premium;
	bool unlocked;
};

struct ProtocolOutfit
{
	ProtocolOutfit(const std::string& name, uint16_t lookType, uint8_t addons) :
	    name(name), lookType(lookType), addons(addons)
	{}

	const std::string& name;
	uint16_t lookType;
	uint8_t addons;
};

struct Look
{
	uint16_t type = 0;
	uint16_t typeEx = 0;
	uint8_t head = 0;
	uint8_t body = 0;
	uint8_t legs = 0;
	uint8_t feet = 0;
	uint8_t addons = 0;
	uint16_t mount = 0;
	uint8_t mountHead = 0;
	uint8_t mountBody = 0;
	uint8_t mountLegs = 0;
	uint8_t mountFeet = 0;
};

#endif
