#include "libs/util/otb/OTBLoadable.h"

#include "libs/item/itemenums.h"
#include "libs/util/otb/otberrors.h"

#include <iostream>

constexpr OTBIdentifier wildcard = {{'\0', '\0', '\0', '\0'}};

OTBLoadable::OTBLoadable(const std::string& fileName, const OTBIdentifier& acceptedIdentifier) :
    fileContents(fileName), fileName(fileName)
{
	constexpr auto minimalSize =
	    sizeof(OTBIdentifier) + sizeof(OTBNode::START) + sizeof(OTBNode::type) + sizeof(OTBNode::END);
	if (fileContents.size() <= minimalSize) {
		throw InvalidOTBFormat{};
	}

	OTBIdentifier fileIdentifier;
	std::copy(fileContents.begin(), fileContents.begin() + fileIdentifier.size(), fileIdentifier.begin());
	if (fileIdentifier != acceptedIdentifier && fileIdentifier != wildcard) {
		throw InvalidOTBFormat{};
	}
}

const OTBNode& OTBLoadable::parseTree()
{
	auto it = fileContents.begin() + sizeof(OTBIdentifier);
	if (static_cast<uint8_t>(*it) != OTBNode::START) {
		throw InvalidOTBFormat{};
	}
	root.type = *(++it);
	root.propsBegin = ++it;

	this->nodeStack = std::stack<OTBNode*, std::vector<OTBNode*>>();
	this->nodeStack.push(&root);

	for (; it != fileContents.end(); ++it) {
		switch (static_cast<uint8_t>(*it)) {
			case OTBNode::START: {
				auto& currentNode = getCurrentNode();
				if (currentNode.children.empty()) {
					currentNode.propsEnd = it;
				}
				currentNode.children.emplace_back();
				auto& child = currentNode.children.back();
				if (++it == fileContents.end()) {
					throw InvalidOTBFormat{};
				}
				child.type = *it;
				child.propsBegin = it + sizeof(OTBNode::type);
				this->nodeStack.push(&child);
				break;
			}
			case OTBNode::END: {
				auto& currentNode = getCurrentNode();
				if (currentNode.children.empty()) {
					currentNode.propsEnd = it;
				}
				this->nodeStack.pop();
				break;
			}
			case OTBNode::ESCAPE: {
				if (++it == fileContents.end()) {
					throw InvalidOTBFormat{};
				}
				break;
			}
			default: {
				break;
			}
		}
	}
	if (!this->nodeStack.empty()) {
		throw InvalidOTBFormat{};
	}

	return root;
}

bool OTBLoadable::getProps(const OTBNode& node, PropStream& props)
{
	auto size = std::distance(node.propsBegin, node.propsEnd);
	if (size == 0) {
		return false;
	}
	propBuffer.resize(size);
	bool lastEscaped = false;

	auto escapedPropEnd =
	    std::copy_if(node.propsBegin, node.propsEnd, propBuffer.begin(), [&lastEscaped](const char& byte) {
		    lastEscaped = byte == static_cast<char>(OTBNode::ESCAPE) && !lastEscaped;
		    return !lastEscaped;
	    });
	props.init(&propBuffer[0], std::distance(propBuffer.begin(), escapedPropEnd));
	return true;
}

bool OTBLoadable::loadFromOTB()
{
	auto& root = this->parseTree();

	PropStream props;
	if (this->getProps(root, props)) {
		// 4 byte flags
		// attributes
		// 0x01 = version data
		uint32_t flags;
		if (!props.read<uint32_t>(flags)) {
			return false;
		}

		uint8_t attr;
		if (!props.read<uint8_t>(attr)) {
			return false;
		}

		if (attr == ROOT_ATTRIBUTE) {
			uint16_t datalen;
			if (!props.read<uint16_t>(datalen)) {
				return false;
			}

			if (datalen != sizeof(VERSIONINFO)) {
				return false;
			}

			VERSIONINFO vi;
			if (!props.read(vi)) {
				return false;
			}

			otbMajorVersion = vi.dwMajorVersion; // items otb format file version
			otbMinorVersion = vi.dwMinorVersion; // client version
			otbBuildNumber = vi.dwBuildNumber;   // revision
		}
	}

	if (otbMajorVersion == 0xFFFFFFFF) {
		std::cout << "[Warning - OTBLoadable::loadFromOtb] " << this->fileName << " using generic client version."
		          << std::endl;
	} else if (otbMajorVersion != 3) {
		std::cout << "Old version detected, a newer version of " << this->fileName << "  is required." << std::endl;
		return false;
	} else if (otbMinorVersion < CLIENT_VERSION_LAST) {
		std::cout << "A newer version of " << this->fileName << "  is required." << std::endl;
		return false;
	}

	for (auto& itemNode : root.children) {
		PropStream stream;
		if (!this->getProps(itemNode, stream)) {
			return false;
		}

		if (!this->load(itemNode, stream)) {
			return false;
		};
	}

	return true;
}
