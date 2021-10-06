#include "objects/channel/wrap_channel.h"

using namespace love;

int Wrap_Channel::Push(lua_State* L)
{
    Channel* self = Wrap_Channel::CheckChannel(L, 1);

    Luax::CatchException(L, [&]() {
        Variant var = Variant::FromLua(L, 2);

        if (var.GetType() == Variant::UNKNOWN)
            luaL_argerror(L, 2, "boolean, number, string, love type, or table expected");

        uint64_t id = self->Push(var);

        lua_pushnumber(L, (lua_Number)id);
    });

    return 1;
}

int Wrap_Channel::Supply(lua_State* L)
{
    Channel* self = Wrap_Channel::CheckChannel(L, 1);
    bool result   = false;

    Luax::CatchException(L, [&]() {
        Variant var = Variant::FromLua(L, 2);

        if (var.GetType() == Variant::UNKNOWN)
            luaL_argerror(L, 2, "boolean, number, string, love type, or table expected");

        if (lua_isnumber(L, 3))
            result = self->Supply(var, lua_tonumber(L, 3));
        else
            result = self->Supply(var);
    });

    lua_pushboolean(L, result);

    return 1;
}

int Wrap_Channel::Pop(lua_State* L)
{
    Channel* self = Wrap_Channel::CheckChannel(L, 1);
    Variant var;

    if (self->Pop(&var))
        var.ToLua(L);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Channel::Demand(lua_State* L)
{
    Channel* self = Wrap_Channel::CheckChannel(L, 1);
    Variant var;
    bool result = false;

    if (lua_isnumber(L, 2))
        result = self->Demand(&var, lua_tonumber(L, 2));
    else
        result = self->Demand(&var);

    if (result)
        var.ToLua(L);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Channel::Peek(lua_State* L)
{
    Channel* self = Wrap_Channel::CheckChannel(L, 1);
    Variant var;

    if (self->Peek(&var))
        var.ToLua(L);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Channel::GetCount(lua_State* L)
{
    Channel* self = Wrap_Channel::CheckChannel(L, 1);

    lua_pushnumber(L, self->GetCount());

    return 1;
}

int Wrap_Channel::HasRead(lua_State* L)
{
    Channel* self = Wrap_Channel::CheckChannel(L, 1);
    uint64_t id   = (uint64_t)luaL_checknumber(L, 2);

    lua_pushboolean(L, self->HasRead(id));

    return 1;
}

int Wrap_Channel::Clear(lua_State* L)
{
    Channel* self = Wrap_Channel::CheckChannel(L, 1);

    self->Clear();

    return 0;
}

int love::Wrap_Channel_PerformAtomic(lua_State* L)
{
    Channel* self = Wrap_Channel::CheckChannel(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    // Pass our channel to function arg
    lua_pushvalue(L, 1);
    lua_insert(L, 3);

    self->LockMutex();

    // Call the function with our channel and args
    int numargs = lua_gettop(L) - 2;
    int error   = lua_pcall(L, numargs, LUA_MULTRET, 0);

    self->UnlockMutex();

    if (error != 0)
        return lua_error(L);

    return lua_gettop(L) - 1;
}

Channel* Wrap_Channel::CheckChannel(lua_State* L, int index)
{
    return Luax::CheckType<Channel>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clear",         Wrap_Channel::Clear                            },
    { "demand",        Wrap_Channel::Demand                           },
    { "getCount",      Wrap_Channel::GetCount                         },
    { "hasRead",       Wrap_Channel::HasRead                          },
    { "peek",          Wrap_Channel::Peek                             },
    { "performAtomic", love::Wrap_Channel_PerformAtomic               },
    { "pop",           Wrap_Channel::Pop                              },
    { "push",          Wrap_Channel::Push                             },
    { "supply",        Wrap_Channel::Supply                           },
    { 0,               0                                              }
};
// clang-format on

int Wrap_Channel::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Channel::type, functions, nullptr);
}
