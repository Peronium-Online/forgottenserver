#ifndef PR_ITEMMAPS_H
#define PR_ITEMMAPS_H

#include "constants/enums.h"
#include "libs/item/itemenums.h"
#include "libs/util/datastructures/Dictionary.h"

#include <boost/algorithm/string.hpp>
#include <unordered_map>

extern Dictionary<ItemFeatures>* ItemParseAttributesMap;
extern Dictionary<ItemTypes>* ItemTypesMap;
extern Dictionary<tileflags_t>* TileStatesMap;
extern Dictionary<RaceType_t>* RaceTypesMap;
extern Dictionary<WeaponTypes>* WeaponTypesMap;
extern Dictionary<FluidTypes>* FluidTypesMap;
extern Dictionary<AmmoTypes>* AmmoTypesMap;

const uint8_t reverseFluidMap[] = {
    FLUID_EMPTY, FLUID_WATER, FLUID_MANA,     FLUID_BEER, FLUID_EMPTY, FLUID_BLOOD,
    FLUID_SLIME, FLUID_EMPTY, FLUID_LEMONADE, FLUID_MILK, FLUID_INK,
};

const uint8_t clientToServerFluidMap[] = {
    FLUID_EMPTY,      FLUID_WATER,       FLUID_MANA, FLUID_BEER, FLUID_MUD,  FLUID_BLOOD, FLUID_SLIME,
    FLUID_RUM,        FLUID_LEMONADE,    FLUID_MILK, FLUID_WINE, FLUID_LIFE, FLUID_URINE, FLUID_OIL,
    FLUID_FRUITJUICE, FLUID_COCONUTMILK, FLUID_TEA,  FLUID_MEAD, FLUID_INK,
};

#endif
