#include "libs/util/tools/date.h"

#include <fmt/chrono.h>

std::string formatDate(time_t time) { return fmt::format("{:%d/%m/%Y %H:%M:%S}", fmt::localtime(time)); }

std::string formatDateShort(time_t time) { return fmt::format("{:%d %b %Y}", fmt::localtime(time)); }

int64_t OTSYS_TIME()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
	    .count();
}
