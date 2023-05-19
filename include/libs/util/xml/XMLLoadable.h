#include "libs/util/tools/xml.h"

#include <iostream>
#include <pugixml.hpp>
#include <string>

#ifndef XMLLOADABLE_H
#define XMLLOADABLE_H

class XMLLoadable
{
public:
	bool isLoaded() const { return loaded; }

	bool loadFromXML(bool reloading = false)
	{
		loaded = false;

		pugi::xml_document doc;
		// std::cout << "Loading XML file: " << this->filepath << std::endl;
		pugi::xml_parse_result result = doc.load_file(this->filepath.c_str());
		if (!result) {
			printXMLError("Error - XMLLoadable::loadFromXML>", this->filepath, result);
			std::cout << "Error - XMLLoadable::loadFromXML " << this->filepath << result << std::endl;
			return false;
		}

		pugi::xml_node rootNode = doc.child(this->childNode.c_str());
		this->loadRootNodeAttributes(rootNode, reloading);
		for (auto node : rootNode.children()) {
			this->load(node, reloading);
		}

		loaded = true;
		return true;
	};

protected:
	std::string filepath;
	std::string childNode;
	bool loaded = false;

	virtual bool load(pugi::xml_node node, bool reloading) = 0;
	virtual bool loadRootNodeAttributes(pugi::xml_node rootNode, bool reloading) { return true; };
};

#endif // XMLLOADABLE_H
