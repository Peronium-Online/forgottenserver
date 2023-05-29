#ifndef FS_RANDOM_H
#define FS_RANDOM_H

#include <cstdint>
#include <random>

std::mt19937& getRandomGenerator();

int32_t uniform_random(int32_t minNumber, int32_t maxNumber);

int32_t normal_random(int32_t minNumber, int32_t maxNumber);

bool boolean_random(double probability = 0.5);

#endif
