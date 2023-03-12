#ifndef FS_HASH_H
#define FS_HASH_H

#include <string>

std::string transformToSHA1(const std::string& input);

std::string generateToken(const std::string& key, uint32_t ticks);

uint32_t adlerChecksum(const uint8_t* data, size_t length);

#endif
