#ifndef PR_REFLECT_ATTRIBUTE
#define PR_REFLECT_ATTRIBUTE

#include <algorithm>
#include <cinttypes>

struct Reflect
{
	Reflect() = default;
	Reflect(uint16_t percent, uint16_t chance) : percent(percent), chance(chance){};

	Reflect& operator+=(const Reflect& other)
	{
		percent += other.percent;
		chance = std::min<uint16_t>(100, chance + other.chance);
		return *this;
	}

	uint16_t percent = 0;
	uint16_t chance = 0;

	static const Reflect UNDEFINED;
};

#endif
