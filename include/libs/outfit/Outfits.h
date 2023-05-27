#ifndef PR_OUTFITS_H
#define PR_OUTFITS_H

#include "constants/enums.h"
#include "libs/outfit/Outfit.h"
#include "libs/util/xml/XMLLoadable.h"

#include <vector>

class Outfits final : virtual public XMLLoadable
{
public:
	Outfits()
	{
		this->filepath = "data/XML/outfits.xml";
		this->childNode = "outfits";
	}

	static Outfits& getInstance()
	{
		static Outfits instance;
		return instance;
	}

	const Outfit* getOutfitByLookType(PlayerSex_t sex, uint16_t lookType) const;
	const Outfit* getOutfitByLookType(uint16_t lookType) const;
	const std::vector<Outfit>& getOutfits(PlayerSex_t sex) const { return outfits[sex]; }

private:
	std::vector<Outfit> outfits[PLAYERSEX_LAST + 1];

	bool load(pugi::xml_node node, bool) override;
};

#endif
