#ifndef PR_ITEMMAPS_H
#define PR_ITEMMAPS_H

#include "../../../src/tile.h"
#include "constants/const.h"
#include "libs/item/itemenums.h"
#include "libs/util/datastructures/Dictionary.h"

#include <boost/algorithm/string.hpp>
#include <unordered_map>

Dictionary<ItemFeatures>* ItemParseAttributesMap;
Dictionary<ItemTypes>* ItemTypesMap;
Dictionary<tileflags_t>* TileStatesMap;
Dictionary<RaceType_t>* RaceTypesMap;
Dictionary<WeaponType_t>* WeaponTypesMap;
Dictionary<FluidTypes_t>* FluidTypesMap;

#endif
