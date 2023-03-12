#ifndef FS_DIRECTION_H
#define FS_DIRECTION_H

#include "../../../../src/position.h"

#include <string>

Direction getDirection(const std::string& string);

Position getNextPosition(Direction direction, Position pos);

Direction getDirectionTo(const Position& from, const Position& to);

#endif
