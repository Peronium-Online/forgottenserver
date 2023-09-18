#ifndef PR_IOMAP_H
#define PR_IOMAP_H

#include "../../../src/configmanager.h"
#include "../../../src/map.h"
#include "libs/util/otb/OTBLoadable.h"

extern ConfigManager g_config;

// TODO: maybe change the name to builder or something like that
class IOMap : virtual public OTBLoadable
{
private:
	// TODO: should be a better way to do this, revist it after full refactor
	std::string errorString;

	Map* map;

	const std::string& fileName;
	bool parseMapDataAttributes(const OTBNode& mapNode, Map& map);
	bool parseWaypoints(const OTBNode& waypointsNode, Map& map);
	bool parseTowns(const OTBNode& townsNode, Map& map);
	bool parseTileArea(const OTBNode& tileAreaNode, Map& map);

	bool load(const OTBNode& node, PropStream stream) override;

public:
	IOMap(const std::string& filename) : OTBLoadable("data/world/" + filename + ".otbm", OTBM), fileName(filename) {}

	static Tile* createTile(Item*& ground, Item* item, uint16_t x, uint16_t y, uint8_t z);

	bool loadSpawns()
	{
		if (map->spawnfile.empty()) {
			// OTBM file doesn't tell us about the spawnfile, lets guess it is mapname-spawn.xml.
			map->spawnfile = g_config.getString(ConfigManager::MAP_NAME);
			map->spawnfile += "-spawn.xml";
		}

		return map->spawns.loadFromXml(map->spawnfile);
	}

	bool loadHouses()
	{
		if (map->housefile.empty()) {
			// OTBM file doesn't tell us about the housefile, lets guess it is mapname-house.xml.
			map->housefile = g_config.getString(ConfigManager::MAP_NAME);
			map->housefile += "-house.xml";
		}

		return map->houses.loadHousesXML(map->housefile);
	}

	const std::string& getLastErrorString() const { return errorString; }

	void setLastErrorString(std::string error) { errorString = error; }

	Map* getMap() { return map; }
};

#endif
