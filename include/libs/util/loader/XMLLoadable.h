#include "libs/util/tools/xml.h"

#include <iostream>
#include <pugixml.hpp>
#include <string>

#ifndef XMLLOADABLE_H
#define XMLLOADABLE_H

class XMLLoadable
{
public:
	bool loadFromXML()
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(this->filepath.c_str());
		if (!result) {
			printXMLError("Error - XMLLoadable::loadFromXml", this->filepath, result);
			std::cout << "Error - XMLLoadable::loadFromXml " << this->filepath << result << std::endl;
			return false;
		}

		for (auto node : doc.child(this->childNode.c_str()).children()) {
			this->load(node);
		}

		return true;
	};

protected:
	std::string filepath;
	std::string childNode;

	virtual bool load(pugi::xml_node node) = 0;
};

#endif // XMLLOADABLE_H
