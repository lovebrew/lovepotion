#include "common/error.hpp"

#include "modules/thread/wrap_Channel.hpp"

using namespace love;

int Wrap_Channel::push(lua_State* L)
{
    auto* self = luax_checkchannel(L, 1);

    luax_catchexcept(L, [&]() {
        auto variant = luax_checkvariant(L, 2);

        if (variant.getType() == Variant::UNKNOWN)
            luaL_argerror(L, 2, E_CHANNEL_VARIANT_UNKNOWN);

        uint64_t id = self->push(variant);
        lua_pushnumber(L, (lua_Number)id);
    });

    return 1;
}

int Wrap_Channel::supply(lua_State* L)
{
    auto* self  = luax_checkchannel(L, 1);
    bool result = false;

    luax_catchexcept(L, [&]() {
        auto variant = luax_checkvariant(L, 2);

        if (variant.getType() == Variant::UNKNOWN)
            luaL_argerror(L, 2, E_CHANNEL_VARIANT_UNKNOWN);

        if (lua_isnumber(L, 3))
            result = self->supply(variant, lua_tonumber(L, 3));
        else
            result = self->supply(variant);
    });

    luax_pushboolean(L, result);

    return 1;
}

int Wrap_Channel::pop(lua_State* L)
{
    auto* self = luax_checkchannel(L, 1);

    Variant variant;

    if (self->pop(&variant))
        luax_pushvariant(L, variant);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Channel::demand(lua_State* L)
{
    auto* self = luax_checkchannel(L, 1);

    Variant variant;
    bool result = false;

    if (lua_isnumber(L, 2))
        result = self->demand(&variant, lua_tonumber(L, 2));
    else
        result = self->demand(&variant);

    if (result)
        luax_pushvariant(L, variant);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Channel::peek(lua_State* L)
{
    auto* self = luax_checkchannel(L, 1);

    Variant variant;

    if (self->peek(&variant))
        luax_pushvariant(L, variant);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Channel::getCount(lua_State* L)
{
    auto* self = luax_checkchannel(L, 1);

    lua_pushnumber(L, self->getCount());

    return 1;
}

int Wrap_Channel::hasRead(lua_State* L)
{
    auto* self  = luax_checkchannel(L, 1);
    uint64_t id = luaL_checknumber(L, 2);

    lua_pushboolean(L, self->hasRead(id));

    return 1;
}

int Wrap_Channel::clear(lua_State* L)
{
    auto* self = luax_checkchannel(L, 1);

    self->clear();

    return 0;
}

int Wrap_Channel::performAtomic(lua_State* L)
{
    auto* self = luax_checkchannel(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    lua_pushvalue(L, 1);
    lua_insert(L, 3);

    int argc  = 0;
    int error = 0;

    {
        /*
        ** pipie does not like The Most Vexing Parse™
        ** (aka std::unique_lock lock (self->getMutex());
        ** this is also known as a skill issue
        */
        std::unique_lock { self->getMutex() };

        argc  = lua_gettop(L) - 2;
        error = lua_pcall(L, argc, LUA_MULTRET, 0);
    }

    if (error != 0)
        return lua_error(L);

    return lua_gettop(L) - 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "push",          Wrap_Channel::push          },
    { "supply",        Wrap_Channel::supply        },
    { "pop",           Wrap_Channel::pop           },
    { "demand",        Wrap_Channel::demand        },
    { "peek",          Wrap_Channel::peek          },
    { "getCount",      Wrap_Channel::getCount      },
    { "hasRead",       Wrap_Channel::hasRead       },
    { "clear",         Wrap_Channel::clear         },
    { "performAtomic", Wrap_Channel::performAtomic }
};
// clang-format on

namespace love
{
    Channel* luax_checkchannel(lua_State* L, int index)
    {
        return luax_checktype<Channel>(L, index);
    }

    int open_channel(lua_State* L)
    {
        return luax_register_type(L, &Channel::type, functions);
    }
} // namespace love
