#ifndef FS_DATE_H
#define FS_DATE_H

#include <string>

std::string formatDate(time_t time);

std::string formatDateShort(time_t time);

int64_t OTSYS_TIME();

#endif
