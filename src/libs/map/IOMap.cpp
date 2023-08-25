#include "libs/map/IOMap.h"

#include "../../housetile.h"
#include "libs/item/ItemAttrSerializer.h"
#include "libs/item/ItemFactory.h"
#include "libs/map/iomapenums.h"
#include "libs/util/tools/propstream.h"

#include <fmt/color.h>

bool IOMap::parseMapDataAttributes(const OTBNode& mapNode, Map& map)
{
	PropStream propStream;
	if (!this->getProps(mapNode, propStream)) {
		setLastErrorString("Could not read map data attributes.");
		return false;
	}

	std::string mapDescription;
	std::string tmp;

	uint8_t attribute;
	while (propStream.read<uint8_t>(attribute)) {
		switch (attribute) {
			case OTBM_ATTR_DESCRIPTION:
				if (!propStream.readString(mapDescription)) {
					setLastErrorString("Invalid description tag.");
					return false;
				}
				break;

			case OTBM_ATTR_EXT_SPAWN_FILE:
				if (!propStream.readString(tmp)) {
					setLastErrorString("Invalid spawn tag.");
					return false;
				}

				map.spawnfile = fileName.substr(0, fileName.rfind('/') + 1);
				map.spawnfile += tmp;
				break;

			case OTBM_ATTR_EXT_HOUSE_FILE:
				if (!propStream.readString(tmp)) {
					setLastErrorString("Invalid house tag.");
					return false;
				}

				map.housefile = fileName.substr(0, fileName.rfind('/') + 1);
				map.housefile += tmp;
				break;

			default:
				setLastErrorString("Unknown header node.");
				return false;
		}
	}
	return true;
}

bool IOMap::parseTileArea(const OTBNode& tileAreaNode, Map& map)
{
	PropStream propStream;
	if (!this->getProps(tileAreaNode, propStream)) {
		setLastErrorString("Invalid map node.");
		return false;
	}

	OTBM_Destination_coords area_coord;
	if (!propStream.read(area_coord)) {
		setLastErrorString("Invalid map node.");
		return false;
	}

	uint16_t base_x = area_coord.x;
	uint16_t base_y = area_coord.y;
	uint16_t z = area_coord.z;

	for (auto& tileNode : tileAreaNode.children) {
		if (tileNode.type != OTBM_TILE && tileNode.type != OTBM_HOUSETILE) {
			setLastErrorString("Unknown tile node.");
			return false;
		}

		if (!this->getProps(tileNode, propStream)) {
			setLastErrorString("Could not read node data.");
			return false;
		}

		OTBM_Tile_coords tile_coord;
		if (!propStream.read(tile_coord)) {
			setLastErrorString("Could not read tile position.");
			return false;
		}

		uint16_t x = base_x + tile_coord.x;
		uint16_t y = base_y + tile_coord.y;

		bool isHouseTile = false;
		House* house = nullptr;
		Tile* tile = nullptr;
		Item* ground_item = nullptr;
		uint32_t tileflags = TILESTATE_NONE;

		if (tileNode.type == OTBM_HOUSETILE) {
			uint32_t houseId;
			if (!propStream.read<uint32_t>(houseId)) {
				setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Could not read house id.", x, y, z));
				return false;
			}

			house = map.houses.addHouse(houseId);
			if (!house) {
				setLastErrorString(
				    fmt::format("[x:{:d}, y:{:d}, z:{:d}] Could not create house id: {:d}", x, y, z, houseId));
				return false;
			}

			tile = new HouseTile(x, y, z, house);
			house->addTile(static_cast<HouseTile*>(tile));
			isHouseTile = true;
		}

		uint8_t attribute;
		// read tile attributes
		while (propStream.read<uint8_t>(attribute)) {
			switch (attribute) {
				case OTBM_ATTR_TILE_FLAGS: {
					uint32_t flags;
					if (!propStream.read<uint32_t>(flags)) {
						setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Failed to read tile flags.", x, y, z));
						return false;
					}

					if ((flags & OTBM_TILEFLAG_PROTECTIONZONE) != 0) {
						tileflags |= TILESTATE_PROTECTIONZONE;
					} else if ((flags & OTBM_TILEFLAG_NOPVPZONE) != 0) {
						tileflags |= TILESTATE_NOPVPZONE;
					} else if ((flags & OTBM_TILEFLAG_PVPZONE) != 0) {
						tileflags |= TILESTATE_PVPZONE;
					}

					if ((flags & OTBM_TILEFLAG_NOLOGOUT) != 0) {
						tileflags |= TILESTATE_NOLOGOUT;
					}
					break;
				}

				case OTBM_ATTR_ITEM: {
					Item* item = ItemFactory::create(propStream);
					if (!item) {
						setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Failed to create item.", x, y, z));
						return false;
					}

					if (isHouseTile && item->isMoveable()) {
						std::cout << "[Warning - IOMap::loadMap] Moveable item with ID: " << item->getID()
						          << ", in house: " << house->getId() << ", at position [x: " << x << ", y: " << y
						          << ", z: " << z << "]." << std::endl;
						delete item;
					} else {
						if (item->getItemCount() == 0) {
							item->setItemCount(1);
						}

						if (tile) {
							tile->internalAddThing(item);
							item->startDecaying();
							item->setLoadedFromMap(true);
						} else if (item->isGroundTile()) {
							delete ground_item;
							ground_item = item;
						} else {
							tile = createTile(ground_item, item, x, y, z);
							tile->internalAddThing(item);
							item->startDecaying();
							item->setLoadedFromMap(true);
						}
					}
					break;
				}

				default:
					setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Unknown tile attribute.", x, y, z));
					return false;
			}
		}

		for (auto& itemNode : tileNode.children) {
			if (itemNode.type != OTBM_ITEM) {
				setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Unknown node type.", x, y, z));
				return false;
			}

			PropStream stream;
			if (!this->getProps(itemNode, stream)) {
				setLastErrorString("Invalid item node.");
				return false;
			}

			Item* item = ItemFactory::create(stream);
			if (!item) {
				setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Failed to create item.", x, y, z));
				return false;
			}

			if (!ItemAttrSerializer::unserializeAttr(item, stream)) {
				setLastErrorString(
				    fmt::format("[x:{:d}, y:{:d}, z:{:d}] Failed to load item {:d}.", x, y, z, item->getID()));
				delete item;
				return false;
			}

			if (isHouseTile && item->isMoveable()) {
				std::cout << "[Warning - IOMap::loadMap] Moveable item with ID: " << item->getID()
				          << ", in house: " << house->getId() << ", at position [x: " << x << ", y: " << y
				          << ", z: " << z << "]." << std::endl;
				delete item;
			} else {
				if (item->getItemCount() == 0) {
					item->setItemCount(1);
				}

				if (tile) {
					tile->internalAddThing(item);
					item->startDecaying();
					item->setLoadedFromMap(true);
				} else if (item->isGroundTile()) {
					delete ground_item;
					ground_item = item;
				} else {
					tile = createTile(ground_item, item, x, y, z);
					tile->internalAddThing(item);
					item->startDecaying();
					item->setLoadedFromMap(true);
				}
			}
		}

		if (!tile) {
			tile = createTile(ground_item, nullptr, x, y, z);
		}

		tile->setFlag(static_cast<tileflags_t>(tileflags));

		map.setTile(x, y, z, tile);
	}
	return true;
}

bool IOMap::parseTowns(const OTBNode& townsNode, Map& map)
{
	for (auto& townNode : townsNode.children) {
		PropStream propStream;
		if (townNode.type != OTBM_TOWN) {
			setLastErrorString("Unknown town node.");
			return false;
		}

		if (!this->getProps(townNode, propStream)) {
			setLastErrorString("Could not read town data.");
			return false;
		}

		uint32_t townId;
		if (!propStream.read<uint32_t>(townId)) {
			setLastErrorString("Could not read town id.");
			return false;
		}

		Town* town = map.towns.getTown(townId);
		if (!town) {
			town = new Town(townId);
			map.towns.addTown(townId, town);
		}

		std::string townName;
		if (!propStream.readString(townName)) {
			setLastErrorString("Could not read town name.");
			return false;
		}

		town->setName(townName);

		OTBM_Destination_coords town_coords;
		if (!propStream.read(town_coords)) {
			setLastErrorString("Could not read town coordinates.");
			return false;
		}

		town->setTemplePos(Position(town_coords.x, town_coords.y, town_coords.z));
	}
	return true;
}

bool IOMap::parseWaypoints(const OTBNode& waypointsNode, Map& map)
{
	PropStream propStream;
	for (auto& node : waypointsNode.children) {
		if (node.type != OTBM_WAYPOINT) {
			setLastErrorString("Unknown waypoint node.");
			return false;
		}

		if (!this->getProps(node, propStream)) {
			setLastErrorString("Could not read waypoint data.");
			return false;
		}

		std::string name;
		if (!propStream.readString(name)) {
			setLastErrorString("Could not read waypoint name.");
			return false;
		}

		OTBM_Destination_coords waypoint_coords;
		if (!propStream.read(waypoint_coords)) {
			setLastErrorString("Could not read waypoint coordinates.");
			return false;
		}

		map.waypoints[name] = Position(waypoint_coords.x, waypoint_coords.y, waypoint_coords.z);
	}
	return true;
}

Tile* IOMap::createTile(Item*& ground, Item* item, uint16_t x, uint16_t y, uint8_t z)
{
	if (!ground) {
		return new StaticTile(x, y, z);
	}

	Tile* tile;
	if ((item && item->isBlocking()) || ground->isBlocking()) {
		tile = new StaticTile(x, y, z);
	} else {
		tile = new DynamicTile(x, y, z);
	}

	tile->internalAddThing(ground);
	ground->startDecaying();
	ground = nullptr;
	return tile;
}
