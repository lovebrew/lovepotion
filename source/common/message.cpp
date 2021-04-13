#include "common/message.h"

using namespace love;

#define FROM_LUA_ERROR \
    "Argument %d can't be stored safely\nExpected boolean, number, string or userdata."

Message::Message(const std::string& name, const std::vector<Variant>& args) : name(name), args(args)
{}

Message::~Message()
{}

Message* Message::FromLua(lua_State* L, int index)
{
    std::string name = luaL_checkstring(L, index);
    std::vector<Variant> vargs;

    int count = lua_gettop(L) - index;
    index++;

    Variant varg;

    for (int i = 0; i < count; i++)
    {
        if (lua_isnoneornil(L, index + i))
            break;

        Luax::CatchException(L, [&]() { vargs.push_back(Variant::FromLua(L, index + i)); });

        if (vargs.back().GetType() == Variant::UNKNOWN)
        {
            vargs.clear();
            luaL_error(L, FROM_LUA_ERROR, index + i);

            return nullptr;
        }
    }

    return new Message(name, vargs);
}

int Message::ToLua(lua_State* L)
{
    Luax::PushString(L, this->name);

    for (const Variant& v : this->args)
        v.ToLua(L);

    return (int)this->args.size() + 1;
}
