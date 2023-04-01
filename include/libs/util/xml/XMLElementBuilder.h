#ifndef PR_XMLELEMENT_H
#define PR_XMLELEMENT_H

#include <pugixml.hpp>

template <typename T>
class XMLElementBuilder
{
public:
	virtual T loadFromXMLNode(pugi::xml_node node, bool reloading) = 0;
};

#endif
