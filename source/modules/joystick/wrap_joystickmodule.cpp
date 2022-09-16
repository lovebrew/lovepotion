#include <modules/joystick/wrap_joystickmodule.hpp>
#include <objects/joystick/wrap_joystick.hpp>

using namespace love;
using JoystickModule = love::JoystickModule<Console::Which>;

#define instance() (Module::GetInstance<::JoystickModule>(Module::M_JOYSTICK))

int Wrap_JoystickModule::GetJoysticks(lua_State* L)
{
    int count = instance()->GetJoystickCount();

    lua_createtable(L, count, 0);
    for (int index = 0; index < count; index++)
    {
        ::Joystick* joystick = instance()->GetJoystick(index);
        luax::PushType(L, joystick);
        lua_rawseti(L, -2, index + 1);
    }

    return 1;
}

int Wrap_JoystickModule::GetIndex(lua_State* L)
{
    return 0;
}

int Wrap_JoystickModule::GetJoystickCount(lua_State* L)
{
    lua_pushinteger(L, instance()->GetJoystickCount());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getJoysticks",     Wrap_JoystickModule::GetJoysticks     },
    { "getJoystickCount", Wrap_JoystickModule::GetJoystickCount },
    { "getIndex",         Wrap_JoystickModule::GetIndex         }
};

static constexpr lua_CFunction types[] =
{
    Wrap_Joystick::Register,
    0
};
// clang-format on

int Wrap_JoystickModule::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new ::JoystickModule(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "joystick";
    wrappedModule.functions = functions;
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = types;

    return luax::RegisterModule(L, wrappedModule);
}
