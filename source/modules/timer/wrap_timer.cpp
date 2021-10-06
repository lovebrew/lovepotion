#include "modules/timer/wrap_timer.h"

using namespace love;

#define instance() (Module::GetInstance<Timer>(Module::M_TIMER))

int Wrap_Timer::GetAverageDelta(lua_State* L)
{
    double average = instance()->GetAverageDelta();

    lua_pushnumber(L, average);

    return 1;
}

int Wrap_Timer::GetDelta(lua_State* L)
{
    float delta = instance()->GetDelta();

    lua_pushnumber(L, delta);

    return 1;
}

int Wrap_Timer::GetFPS(lua_State* L)
{
    int fps = instance()->GetFPS();

    lua_pushinteger(L, fps);

    return 1;
}

int Wrap_Timer::GetTime(lua_State* L)
{
    double time = instance()->GetTime();

    lua_pushnumber(L, time);

    return 1;
}

int Wrap_Timer::Sleep(lua_State* L)
{
    float seconds = luaL_checknumber(L, 1);

    instance()->Sleep(seconds);

    return 0;
}

int Wrap_Timer::Step(lua_State* L)
{
    double dt = instance()->Step();

    lua_pushnumber(L, dt);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getAverageDelta", Wrap_Timer::GetAverageDelta },
    { "getDelta",        Wrap_Timer::GetDelta        },
    { "getFPS",          Wrap_Timer::GetFPS          },
    { "getTime",         Wrap_Timer::GetTime         },
    { "sleep",           Wrap_Timer::Sleep           },
    { "step",            Wrap_Timer::Step            },
    { 0,                 0                           }
};
// clang-format on

int Wrap_Timer::Register(lua_State* L)
{
    Timer* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Timer(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "timer";
    wrappedModule.functions = functions;
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = nullptr;

    return Luax::RegisterModule(L, wrappedModule);
}
