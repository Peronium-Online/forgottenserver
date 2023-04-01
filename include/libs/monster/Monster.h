#ifndef PR_MONSTER_H
#define PR_MONSTER_H

#include "libs/monster/MonsterType.h"
#include "libs/util/xml/XMLLoadable.h"

#include <string>

class Monster final : virtual public XMLLoadable
{
public:
	Monster();
	Monster(std::string filepath, std::string childNode)
	{
		this->filepath = filepath;
		this->childNode = childNode;
	};
	explicit Monster(MonsterType* mType);
	~Monster();

	// non-copyable
	Monster(const Monster&) = delete;
	Monster& operator=(const Monster&) = delete;

private:
	MonsterType* mType;

	virtual bool load(pugi::xml_node node, bool reloading) override;
};

#endif
