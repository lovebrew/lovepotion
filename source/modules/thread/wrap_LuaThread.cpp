#include "common/error.hpp"

#include "modules/thread/wrap_LuaThread.hpp"

using namespace love;

int Wrap_Thread::start(lua_State* L)
{
    auto* self = luax_checkthread(L, 1);

    std::vector<Variant> args {};
    int argc = lua_gettop(L) - 1;

    for (int index = 0; index < argc; ++index)
    {
        luax_catchexcept(L, [&]() { args.push_back(luax_checkvariant(L, index + 2)); });

        if (args.back().getType() == Variant::UNKNOWN)
        {
            args.clear();
            return luaL_argerror(L, index + 2, E_THREAD_VARIANT_UNKNOWN);
        }
    }

    luax_pushboolean(L, self->start(args));

    return 1;
}

int Wrap_Thread::wait(lua_State* L)
{
    auto* self = luax_checkthread(L, 1);

    self->wait();

    return 0;
}

int Wrap_Thread::getError(lua_State* L)
{
    auto* self = luax_checkthread(L, 1);

    if (self->hasError())
        luax_pushstring(L, self->getError());
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Thread::isRunning(lua_State* L)
{
    auto* self = luax_checkthread(L, 1);

    luax_pushboolean(L, self->isRunning());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "start",      Wrap_Thread::start     },
    { "wait",       Wrap_Thread::wait      },
    { "getError",   Wrap_Thread::getError  },
    { "isRunning",  Wrap_Thread::isRunning }
};
// clang-format on

namespace love
{
    LuaThread* luax_checkthread(lua_State* L, int index)
    {
        return luax_checktype<LuaThread>(L, index);
    }

    int open_thread(lua_State* L)
    {
        return luax_register_type(L, &LuaThread::type, functions);
    }
} // namespace love
