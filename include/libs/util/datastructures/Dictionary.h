#ifndef PR_UNORDORED_MAP
#define PR_UNORDORED_MAP

#include <unordered_map>

template <typename T>
class Dictionary
{
private:
	const std::unordered_map<std::string, T> map;
	const T NONE_VALUE;

public:
	Dictionary(const std::unordered_map<std::string, T>& map, T noneValue) : map(map), NONE_VALUE(noneValue){};

	T get(const std::string& key)
	{
		std::string tmpStrValue = boost::algorithm::to_lower_copy<std::string>(key);
		auto it = map.find(tmpStrValue);
		if (it != map.end()) {
			return it->second;
		}

		return NONE_VALUE;
	}

	bool isNone(T value) { return value == NONE_VALUE; }
};

#endif
