#include "modules/timer/wrap_Timer.hpp"
#include "modules/timer/Timer.hpp"

using namespace love;

#define instance() (Module::getInstance<Timer>(Module::M_TIMER))

int Wrap_Timer::step(lua_State* L)
{
    lua_pushnumber(L, instance()->step());

    return 1;
}

int Wrap_Timer::getDelta(lua_State* L)
{
    lua_pushnumber(L, instance()->getDelta());

    return 1;
}

int Wrap_Timer::getFPS(lua_State* L)
{
    lua_pushinteger(L, (int)instance()->getFPS());

    return 1;
}

int Wrap_Timer::getAverageDelta(lua_State* L)
{
    lua_pushnumber(L, instance()->getAverageDelta());

    return 1;
}

int Wrap_Timer::sleep(lua_State* L)
{
    instance()->sleep(luaL_checknumber(L, 1));

    return 0;
}

int Wrap_Timer::getTime(lua_State* L)
{
    lua_pushnumber(L, instance()->getTime());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "step",            Wrap_Timer::step            },
    { "getDelta",        Wrap_Timer::getDelta        },
    { "getFPS",          Wrap_Timer::getFPS          },
    { "getAverageDelta", Wrap_Timer::getAverageDelta },
    { "sleep",           Wrap_Timer::sleep           },
    { "getTime",         Wrap_Timer::getTime         }
};
// clang-format on

int Wrap_Timer::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new Timer(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "timer";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = {};

    return luax_register_module(L, module);
}
