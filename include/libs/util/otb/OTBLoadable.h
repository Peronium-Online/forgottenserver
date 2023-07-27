#ifndef PR_OTBLOADABLE_H
#define PR_OTBLOADABLE_H

#include "libs/util/otb/otberrors.h"
#include "libs/util/tools/propstream.h"

#include <array>
#include <boost/iostreams/device/mapped_file.hpp>
#include <stack>

using MappedFile = boost::iostreams::mapped_file_source;
using ContentIt = MappedFile::iterator;
using OTBIdentifier = std::array<char, 4>;

constexpr auto OTBI = OTBIdentifier{{'O', 'T', 'B', 'I'}};

// 1-byte aligned structs
#pragma pack(1)

struct VERSIONINFO
{
	uint32_t dwMajorVersion;
	uint32_t dwMinorVersion;
	uint32_t dwBuildNumber;
	uint8_t CSDVersion[128];
};

struct lightBlock2
{
	uint16_t lightLevel;
	uint16_t lightColor;
};

#pragma pack()

struct OTBNode
{
	using ChildrenVector = std::vector<OTBNode>;

	ChildrenVector children;
	ContentIt propsBegin;
	ContentIt propsEnd;
	uint8_t type;
	enum NodeChar : uint8_t
	{
		ESCAPE = 0xFD,
		START = 0xFE,
		END = 0xFF,
	};
};

class OTBLoadable
{
private:
	const std::string& fileName;
	MappedFile fileContents;
	OTBNode root;
	std::vector<char> propBuffer;
	std::stack<OTBNode*, std::vector<OTBNode*>> nodeStack;

	OTBNode& getCurrentNode()
	{
		if (nodeStack.empty()) {
			throw InvalidOTBFormat{};
		}
		return *nodeStack.top();
	}

	virtual bool load(const OTBNode& node, PropStream stream) = 0;

protected:
	OTBLoadable(const std::string& fileName, const OTBIdentifier& acceptedIdentifier);

	bool getProps(const OTBNode& node, PropStream& props);
	const OTBNode& parseTree();

public:
	uint32_t otbMajorVersion = 0;
	uint32_t otbMinorVersion = 0;
	uint32_t otbBuildNumber = 0;

	virtual bool loadFromOTB();
};

#endif
