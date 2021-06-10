#include "modules/physics/wrap_physics.h"

using namespace love;

#define instance() (Module::GetInstance<Physics>(Module::M_PHYSICS))

int Wrap_Physics::GetDistance(lua_State* L)
{
    return instance()->GetDistance(L);
}

int Wrap_Physics::SetMeter(lua_State* L)
{
    float meter = luaL_checknumber(L, 1);
    Luax::CatchException(L, [&]() { Physics::SetMeter(meter); });

    return 0;
}

int Wrap_Physics::GetMeter(lua_State* L)
{
    lua_pushinteger(L, Physics::GetMeter());

    return 1;
}

int Wrap_Physics::Register(lua_State* L)
{
    // clang-format off
    luaL_Reg funcs[] =
    {
        { "getMeter", GetMeter },
        { "setMeter", SetMeter },
        { 0,          0        }
    };

    lua_CFunction types[] =
    {
        Wrap_Contact::Register,
        Wrap_Fixture::Register,
        Wrap_Shape::Register,
        0
    };
    // clang-format on

    Physics* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Physics(); });
    else
        instance->Retain();

    WrappedModule module;
    module.instance  = instance;
    module.name      = "physics";
    module.type      = &Module::type;
    module.functions = funcs;
    module.types     = types;

    return Luax::RegisterModule(L, module);
}
