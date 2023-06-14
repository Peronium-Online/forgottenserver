#ifndef PR_CUSTOM_ATTRIBUTE_H
#define PR_CUSTOM_ATTRIBUTE_H

#include "../../../../src/luascript.h"
#include "libs/util/tools/propstream.h"

#include <boost/variant.hpp>

#if __has_include("luajit/lua.hpp")
#include <luajit/lua.hpp>
#else
#include <lua.hpp>
#endif

struct CustomLuaAttribute
{
	typedef boost::variant<boost::blank, std::string, int64_t, double, bool> VariantAttribute;
	VariantAttribute value;

	CustomLuaAttribute() : value(boost::blank()) {}

	bool operator==(const CustomLuaAttribute& otherAttr) const { return value == otherAttr.value; }

	bool operator!=(const CustomLuaAttribute& otherAttr) const { return value != otherAttr.value; }

	template <typename T>
	explicit CustomLuaAttribute(const T& v) : value(v)
	{}

	template <typename T>
	void set(const T& v)
	{
		value = v;
	}

	template <typename T>
	const T& get();

	struct PushLuaVisitor : public boost::static_visitor<>
	{
		lua_State* L;

		explicit PushLuaVisitor(lua_State* L) : boost::static_visitor<>(), L(L) {}

		void operator()(const boost::blank&) const { lua_pushnil(L); }

		void operator()(const std::string& v) const { LuaScriptInterface::pushString(L, v); }

		void operator()(bool v) const { LuaScriptInterface::pushBoolean(L, v); }

		void operator()(const int64_t& v) const { lua_pushnumber(L, v); }

		void operator()(const double& v) const { lua_pushnumber(L, v); }
	};

	void pushToLua(lua_State* L) const { boost::apply_visitor(PushLuaVisitor(L), value); }

	struct SerializeVisitor : public boost::static_visitor<>
	{
		PropWriteStream& propWriteStream;

		explicit SerializeVisitor(PropWriteStream& propWriteStream) :
		    boost::static_visitor<>(), propWriteStream(propWriteStream)
		{}

		void operator()(const boost::blank&) const {}

		void operator()(const std::string& v) const { propWriteStream.writeString(v); }

		template <typename T>
		void operator()(const T& v) const
		{
			propWriteStream.write<T>(v);
		}
	};

	void serialize(PropWriteStream& propWriteStream) const
	{
		propWriteStream.write<uint8_t>(static_cast<uint8_t>(value.which()));
		boost::apply_visitor(SerializeVisitor(propWriteStream), value);
	}

	bool unserialize(PropStream& propStream)
	{
		// This is hard-coded so it's not general, depends on the position of the variants.
		uint8_t pos;
		if (!propStream.read<uint8_t>(pos)) {
			return false;
		}

		switch (pos) {
			case 1: { // std::string
				std::string tmp;
				if (!propStream.readString(tmp)) {
					return false;
				}
				value = tmp;
				break;
			}

			case 2: { // int64_t
				int64_t tmp;
				if (!propStream.read<int64_t>(tmp)) {
					return false;
				}
				value = tmp;
				break;
			}

			case 3: { // double
				double tmp;
				if (!propStream.read<double>(tmp)) {
					return false;
				}
				value = tmp;
				break;
			}

			case 4: { // bool
				bool tmp;
				if (!propStream.read<bool>(tmp)) {
					return false;
				}
				value = tmp;
				break;
			}

			default: {
				value = boost::blank();
				return false;
			}
		}
		return true;
	}
};

#endif
