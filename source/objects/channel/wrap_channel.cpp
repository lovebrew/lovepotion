#include <common/variant.hpp>

#include <objects/channel/wrap_channel.hpp>

using namespace love;

Channel* Wrap_Channel::CheckChannel(lua_State* L, int index)
{
    return luax::CheckType<Channel>(L, index);
}

int Wrap_Channel::Push(lua_State* L)
{
    auto* self = Wrap_Channel::CheckChannel(L, 1);

    luax::CatchException(L, [&]() {
        love::Variant variant = luax::CheckVariant(L, 2);

        if (variant.GetType() == love::Variant::UNKNOWN)
            luaL_argerror(L, 2, "boolean, number, string, love type, or table expected");

        uint64_t id = self->Push(variant);
        lua_pushnumber(L, (lua_Number)id);
    });

    return 1;
}

int Wrap_Channel::Supply(lua_State* L)
{
    auto* self  = Wrap_Channel::CheckChannel(L, 1);
    bool result = false;

    luax::CatchException(L, [&]() {
        love::Variant variant = luax::CheckVariant(L, 2);

        if (variant.GetType() == love::Variant::UNKNOWN)
            luaL_argerror(L, 2, "boolean, number, string, love type, or table expected");

        if (lua_isnumber(L, 3))
            result = self->Supply(variant, lua_tonumber(L, 3));
        else
            result = self->Supply(variant);
    });

    luax::PushBoolean(L, result);

    return 1;
}

int Wrap_Channel::Pop(lua_State* L)
{
    auto* self = Wrap_Channel::CheckChannel(L, 1);
    love::Variant variant;

    if (self->Pop(&variant))
        luax::PushVariant(L, variant);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Channel::Demand(lua_State* L)
{
    auto* self = Wrap_Channel::CheckChannel(L, 1);

    love::Variant variant;
    bool result = false;

    if (lua_isnumber(L, 2))
        result = self->Demand(&variant, lua_tonumber(L, 2));
    else
        result = self->Demand(&variant);

    if (result)
        luax::PushVariant(L, variant);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Channel::Peek(lua_State* L)
{
    auto* self = Wrap_Channel::CheckChannel(L, 1);
    love::Variant variant;

    if (self->Peek(&variant))
        luax::PushVariant(L, variant);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Channel::Clear(lua_State* L)
{
    auto* self = Wrap_Channel::CheckChannel(L, 1);
    self->Clear();

    return 0;
}

int Wrap_Channel::GetCount(lua_State* L)
{
    auto* self = Wrap_Channel::CheckChannel(L, 1);

    lua_pushnumber(L, self->GetCount());
    return 1;
}

int Wrap_Channel::HasRead(lua_State* L)
{
    auto* self  = Wrap_Channel::CheckChannel(L, 1);
    uint64_t id = luaL_checknumber(L, 2);

    luax::PushBoolean(L, self->HasRead(id));

    return 1;
}

int Wrap_Channel::PerformAtomic(lua_State* L)
{
    auto* self = Wrap_Channel::CheckChannel(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    /* pass channel as argument */
    lua_pushvalue(L, 1);
    lua_insert(L, 3);

    self->LockMutex();

    /* call the function and its args */
    int argCount = lua_gettop(L) - 2;
    int error    = lua_pcall(L, argCount, LUA_MULTRET, 0);

    self->UnlockMutex();

    if (error != 0)
        return lua_error(L);

    return lua_gettop(L) - 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clear",         Wrap_Channel::Clear         },
    { "demand",        Wrap_Channel::Demand        },
    { "getCount",      Wrap_Channel::GetCount      },
    { "hasRead",       Wrap_Channel::HasRead       },
    { "peek",          Wrap_Channel::Peek          },
    { "performAtomic", Wrap_Channel::PerformAtomic },
    { "pop",           Wrap_Channel::Pop           },
    { "push",          Wrap_Channel::Push          },
    { "supply",        Wrap_Channel::Supply        }
};
// clang-format on

int Wrap_Channel::Register(lua_State* L)
{
    return luax::RegisterType(L, &Channel::type, functions);
}
