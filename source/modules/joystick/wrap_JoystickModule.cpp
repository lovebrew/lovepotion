#include "modules/joystick/wrap_JoystickModule.hpp"
#include "modules/joystick/wrap_Joystick.hpp"

using namespace love;

#define instance() Module::getInstance<JoystickModule>(Module::M_JOYSTICK)

int Wrap_JoystickModule::getJoysticks(lua_State* L)
{
    int count = instance()->getJoystickCount();
    lua_createtable(L, count, 0);

    for (int index = 0; index < count; index++)
    {
        auto* joystick = instance()->getJoystick(index);
        luax_pushtype(L, joystick);
        lua_rawseti(L, -2, index + 1);
    }

    return 1;
}

int Wrap_JoystickModule::getIndex(lua_State* L)
{
    auto* joystick = luax_checkjoystick(L, 1);
    int index      = instance()->getIndex(joystick);

    if (index >= 0)
        lua_pushinteger(L, index);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_JoystickModule::getJoystickCount(lua_State* L)
{
    lua_pushinteger(L, instance()->getJoystickCount());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getJoysticks",     Wrap_JoystickModule::getJoysticks     },
    { "getIndex",         Wrap_JoystickModule::getIndex         },
    { "getJoystickCount", Wrap_JoystickModule::getJoystickCount }
};

static constexpr lua_CFunction types[] =
{
    love::open_joystick
};
// clang-format on

int Wrap_JoystickModule::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&] { instance = new JoystickModule(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "joystick";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return luax_register_module(L, module);
}
