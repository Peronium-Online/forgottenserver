#ifndef FS_STRINGS_H
#define FS_STRINGS_H

#include <string>
#include <vector>

using StringVector = std::vector<std::string>;
using IntegerVector = std::vector<int32_t>;

// checks that str1 is equivalent to str2 ignoring letter case
bool caseInsensitiveEqual(std::string_view str1, std::string_view str2);

// checks that str1 starts with str2 ignoring letter case
bool caseInsensitiveStartsWith(std::string_view str, std::string_view prefix);

StringVector explodeString(const std::string& inString, const std::string& separator, int32_t limit = -1);

IntegerVector vectorAtoi(const StringVector& stringVector);

std::string getFirstLine(const std::string& str);

std::string ucfirst(std::string str);

std::string ucwords(std::string str);

bool booleanString(const std::string& str);

#endif
