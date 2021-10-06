#include "objects/thread/wrap_luathread.h"

using namespace love;

int Wrap_LuaThread::Start(lua_State* L)
{
    LuaThread* self = Wrap_LuaThread::CheckThread(L, 1);
    std::vector<Variant> args;

    int nargs = lua_gettop(L) - 1;

    for (int i = 0; i < nargs; ++i)
    {
        Luax::CatchException(L, [&]() { args.push_back(Variant::FromLua(L, i + 2)); });

        if (args.back().GetType() == Variant::UNKNOWN)
        {
            args.clear();
            return luaL_argerror(L, i + 2,
                                 "boolean, number, string, love type, or flat table expected");
        }
    }

    lua_pushboolean(L, self->Start(args));

    return 1;
}

int Wrap_LuaThread::Wait(lua_State* L)
{
    LuaThread* self = Wrap_LuaThread::CheckThread(L, 1);
    self->Wait();

    return 0;
}

int Wrap_LuaThread::GetError(lua_State* L)
{
    LuaThread* self   = Wrap_LuaThread::CheckThread(L, 1);
    std::string error = self->GetError();

    if (error.empty())
        lua_pushnil(L);
    else
        Luax::PushString(L, error);

    return 1;
}

int Wrap_LuaThread::IsRunning(lua_State* L)
{
    LuaThread* self = Wrap_LuaThread::CheckThread(L, 1);

    lua_pushboolean(L, self->IsRunning());

    return 1;
}

LuaThread* Wrap_LuaThread::CheckThread(lua_State* L, int index)
{
    return Luax::CheckType<LuaThread>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getError",  Wrap_LuaThread::GetError  },
    { "isRunning", Wrap_LuaThread::IsRunning },
    { "start",     Wrap_LuaThread::Start     },
    { "wait",      Wrap_LuaThread::Wait      },
    { 0,           0                         }
};
// clang-format on

int Wrap_LuaThread::Register(lua_State* L)
{
    return Luax::RegisterType(L, &LuaThread::type, functions, nullptr);
}
