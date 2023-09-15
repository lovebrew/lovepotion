#include <objects/thread/wrap_luathread.hpp>

using namespace love;

LuaThread* Wrap_LuaThread::CheckThread(lua_State* L, int index)
{
    return luax::CheckType<LuaThread>(L, index);
}

int Wrap_LuaThread::Start(lua_State* L)
{
    LuaThread* self = Wrap_LuaThread::CheckThread(L, 1);
    std::vector<Variant> args;

    int argCount = lua_gettop(L) - 1;

    for (int index = 0; index < argCount; ++index)
    {
        luax::CatchException(L, [&]() { args.push_back(luax::CheckVariant(L, index + 2)); });

        if (args.back().GetType() == Variant::UNKNOWN)
        {
            args.clear();
            return luaL_argerror(L, index + 1,
                                 "boolean, number, string, love type, or flat table expected.");
        }
    }

    luax::CatchException(L, [&]() { luax::PushBoolean(L, self->Start(args)); });

    return 1;
}

int Wrap_LuaThread::IsRunning(lua_State* L)
{
    LuaThread* self = Wrap_LuaThread::CheckThread(L, 1);

    luax::PushBoolean(L, self->IsRunning());

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
    LuaThread* self = Wrap_LuaThread::CheckThread(L, 1);

    if (self->HasError())
        luax::PushString(L, self->GetError());
    else
        lua_pushnil(L);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getError",  Wrap_LuaThread::GetError  },
    { "isRunning", Wrap_LuaThread::IsRunning },
    { "start",     Wrap_LuaThread::Start     },
    { "wait",      Wrap_LuaThread::Wait      }
};
// clang-format on

int Wrap_LuaThread::Register(lua_State* L)
{
    return luax::RegisterType(L, &LuaThread::type, functions);
}
