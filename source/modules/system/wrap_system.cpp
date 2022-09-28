#include <modules/system/wrap_system.hpp>
#include <modules/system_ext.hpp>

#if !defined(__3DS__)
std::span<const luaL_Reg> Wrap_System::extensions;
#endif

using namespace love;

#define instance() (Module::GetInstance<System<Console::Which>>(Module::M_SYSTEM))

int Wrap_System::GetOS(lua_State* L)
{
    auto osName = instance()->GetOS();

    luax::PushString(L, osName);

    return 1;
}

int Wrap_System::GetProcessorCount(lua_State* L)
{
    lua_pushinteger(L, instance()->GetProcessorCount());

    return 1;
}

int Wrap_System::GetPowerInfo(lua_State* L)
{
    uint8_t percent = -1;

    auto state = instance()->GetPowerInfo(percent);

    if (auto found = System<>::powerStates.ReverseFind(state))
        lua_pushstring(L, *found);
    else
        lua_pushstring(L, "unknown");

    if (percent >= 0)
        lua_pushinteger(L, percent);
    else
        lua_pushnil(L);

    lua_pushnil(L);

    return 3;
}

int Wrap_System::GetNetworkInfo(lua_State* L)
{
    uint8_t signal  = -1;

    auto state = instance()->GetNetworkInfo(signal);

    if (auto found = System<>::networkStates.ReverseFind(state))
        lua_pushstring(L, *found);
    else
        lua_pushstring(L, "unknown");

    lua_pushinteger(L, signal);

    return 2;
}

int Wrap_System::GetPreferredLocales(lua_State* L)
{
    auto locale = instance()->GetPreferredLocales();

    lua_createtable(L, 1, 0);
    luax::PushString(L, locale);
    lua_rawseti(L, -2, 1);

    return 1;
}

int Wrap_System::GetModel(lua_State* L)
{
    auto model = instance()->GetModel();

    luax::PushString(L, model);

    return 1;
}

int Wrap_System::GetUsername(lua_State* L)
{
    auto username = instance()->GetUsername();

    luax::PushString(L, username);

    return 1;
}

int Wrap_System::GetVersion(lua_State* L)
{
    auto version = instance()->GetVersion();

    luax::PushString(L, version);

    return 1;
}

int Wrap_System::GetFriendInfo(lua_State* L)
{
    auto friendCode = instance()->GetFriendInfo();

    luax::PushString(L, friendCode);

    return 1;
}

int Wrap_System::GetSystemTheme(lua_State* L)
{
    auto theme = instance()->GetSystemTheme();

    luax::PushString(L, theme);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getColorTheme",       Wrap_System::GetSystemTheme      },
    { "getFriendInfo",       Wrap_System::GetFriendInfo       },
    { "getPreferredLocales", Wrap_System::GetPreferredLocales },
    { "getModel",            Wrap_System::GetModel            },
    { "getNetworkInfo",      Wrap_System::GetNetworkInfo      },
    { "getOS",               Wrap_System::GetOS               },
    { "getPowerInfo",        Wrap_System::GetPowerInfo        },
    { "getProcessorCount",   Wrap_System::GetProcessorCount   },
    { "getUsername",         Wrap_System::GetUsername         },
    { "getVersion",          Wrap_System::GetVersion          }
};
// clang-format on

int Wrap_System::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new System<Console::Which>(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance          = instance;
    wrappedModule.name              = "system";
    wrappedModule.type              = &Module::type;
    wrappedModule.functions         = functions;
    wrappedModule.extendedFunctions = extensions;
    wrappedModule.types             = nullptr;

    return luax::RegisterModule(L, wrappedModule);
}
