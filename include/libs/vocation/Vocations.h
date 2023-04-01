#ifndef FS_VOCATIONS_H
#define FS_VOCATIONS_H

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
	~Vocations(){};

	Vocation* getVocation(uint16_t id);
	int32_t getVocationId(const std::string& name) const;
	uint16_t getPromotedVocation(uint16_t vocationId) const;

private:
	std::map<uint16_t, Vocation> vocationsMap;

	virtual bool load(pugi::xml_node node, bool) override;
};

#endif // FS_VOCATIONS_H
