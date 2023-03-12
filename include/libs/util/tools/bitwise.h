#ifndef FS_BITWISE_H
#define FS_BITWISE_H

#include <cinttypes>

constexpr bool hasBitSet(uint32_t flag, uint32_t flags) { return (flags & flag) != 0; }

#endif
