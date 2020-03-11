#include "common/runtime.h"
#include "modules/timer/wrap_timer.h"

using namespace love;

#define instance() (Module::GetInstance<Timer>(Module::M_TIMER))

int Wrap_Timer::GetAverageDelta(lua_State * L)
{
    double average = instance()->GetAverageDelta();

    lua_pushnumber(L, average);

    return 1;
}

int Wrap_Timer::GetDelta(lua_State * L)
{
    float delta = instance()->GetDelta();

    lua_pushnumber(L, delta);

    return 1;
}

int Wrap_Timer::GetFPS(lua_State * L)
{
    int fps = instance()->GetFPS();

    lua_pushinteger(L, fps);

    return 1;
}

int Wrap_Timer::GetTime(lua_State * L)
{
    double time = instance()->GetTime();

    lua_pushnumber(L, time);

    return 1;
}

int Wrap_Timer::Sleep(lua_State * L)
{
    float seconds = luaL_checknumber(L, 1);

    instance()->Sleep(seconds);

    return 0;
}

int Wrap_Timer::Step(lua_State * L)
{
    double dt = instance()->Step();

    lua_pushnumber(L, dt);

    return 1;
}

int Wrap_Timer::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "getAverageDelta", GetAverageDelta },
        { "getDelta",        GetDelta        },
        { "getFPS",          GetFPS          },
        { "getTime",         GetTime         },
        { "sleep",           Sleep           },
        { "step",            Step            },
        { 0,                 0               }
    };

    Timer * instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Timer(); });
    else
        instance->Retain();

    WrappedModule module;

    module.instance = instance;
    module.name = "timer";
    module.functions = reg;
    module.type = &Module::type;
    module.types = 0;

    return Luax::RegisterModule(L, module);
}
