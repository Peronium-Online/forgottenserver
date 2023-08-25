#ifndef PR_IOMAP_H
#define PR_IOMAP_H

#include "../../src/map.h"
#include "libs/util/otb/OTBLoadable.h"

class IOMap : virtual public OTBLoadable
{
private:
	bool parseMapDataAttributes(const OTBNode& mapNode, Map& map);
	bool parseWaypoints(const OTBNode& waypointsNode, Map& map);
	bool parseTowns(const OTBNode& townsNode, Map& map);
	bool parseTileArea(const OTBNode& tileAreaNode, Map& map);

	// TODO: should be a better way to do this, revist it after full refactor
	std::string errorString;

	const std::string& fileName;

public:
	IOMap(const std::string& filename) : OTBLoadable("data/world/" + filename + ".otbm", OTBM), fileName(filename) {}

	static Tile* createTile(Item*& ground, Item* item, uint16_t x, uint16_t y, uint8_t z);

	const std::string& getLastErrorString() const { return errorString; }

	void setLastErrorString(std::string error) { errorString = error; }
};

#endif
