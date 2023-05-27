#ifndef PR_VOCATIONS_H
#define PR_VOCATIONS_H

#include "libs/util/xml/XMLLoadable.h"
#include "libs/vocation/Vocation.h"

#include <map>

class Vocations final : virtual public XMLLoadable
{
public:
	Vocations()
	{
		this->filepath = "data/XML/vocations.xml";
		this->childNode = "vocations";
	};

	Vocation* getVocation(uint16_t id);
	int32_t getVocationId(const std::string& name) const;
	uint16_t getPromotedVocation(uint16_t vocationId) const;

private:
	std::map<uint16_t, Vocation> vocationsMap;

	bool load(pugi::xml_node node, bool) override;
};

#endif
