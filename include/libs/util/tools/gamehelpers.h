#ifndef FS_GAMEHELPERS_H
#define FS_GAMEHELPERS_H

#include "constants/const.h"
#include "constants/enums.h"
#include "libs/item/itemenums.h"

#include <string>

uint16_t getDepotBoxId(uint16_t index);

MagicEffectClasses getMagicEffect(const std::string& strValue);

ShootType_t getShootType(const std::string& strValue);

AmmoTypes getAmmoType(const std::string& strValue);

WeaponAction_t getWeaponAction(const std::string& strValue);

Skulls_t getSkullType(const std::string& strValue);

std::string getCombatName(CombatType_t combatType);

std::string getSpecialSkillName(uint8_t skillid);

std::string getSkillName(uint8_t skillid);

size_t combatTypeToIndex(CombatType_t combatType);

CombatType_t indexToCombatType(size_t v);

uint8_t serverFluidToClient(uint8_t serverFluid);

uint8_t clientFluidToServer(uint8_t clientFluid);

SpellGroup_t stringToSpellGroup(const std::string& value);

const char* getReturnMessage(ReturnValue value);

#endif
