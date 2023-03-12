#ifndef FS_XML_H
#define FS_XML_H

#include <pugixml.hpp>
#include <string>

void printXMLError(const std::string& where, const std::string& fileName, const pugi::xml_parse_result& result);

#endif
