#include "modules/system/wrap_System.hpp"

using namespace love;

#define instance() Module::getInstance<System>(Module::M_SYSTEM)

int Wrap_System::getProcessorCount(lua_State* L)
{
    lua_pushinteger(L, instance()->getProcessorCount());

    return 1;
}

int Wrap_System::getPowerInfo(lua_State* L)
{
    int seconds = 0;
    int percent = 0;

    auto powerState = instance()->getPowerInfo(seconds, percent);

    std::string_view state {};
    if (!System::getConstant(powerState, state))
        state = "unknown";

    luax_pushstring(L, state);
    lua_pushinteger(L, percent);
    lua_pushnil(L);

    return 3;
}

int Wrap_System::getClipboardText(lua_State* L)
{
    luax_pushstring(L, instance()->getClipboardText());

    return 1;
}

int Wrap_System::setClipboardText(lua_State* L)
{
    instance()->setClipboardText(luax_checkstring(L, 1));

    return 0;
}

int Wrap_System::vibrate(lua_State*)
{
    return 0;
}

int Wrap_System::openURL(lua_State*)
{
    return 0;
}

int Wrap_System::hasBackgroundMusic(lua_State* L)
{
    lua_pushboolean(L, false);

    return 1;
}

int Wrap_System::getPreferredLocales(lua_State* L)
{
    auto locales = instance()->getPreferredLocales();

    lua_createtable(L, locales.size(), 0);

    for (size_t index = 0; index < locales.size(); ++index)
    {
        luax_pushstring(L, locales[index]);
        lua_rawseti(L, -2, index + 1);
    }

    return 1;
}

int Wrap_System::getOS(lua_State* L)
{
    luax_pushstring(L, instance()->getOS());

    return 1;
}

int Wrap_System::getNetworkInfo(lua_State* L)
{
    uint8_t signal = 0;

    auto networkState = instance()->getNetworkInfo(signal);

    std::string_view state {};
    if (!System::getConstant(networkState, state))
        state = "unknown";

    luax_pushstring(L, state);
    lua_pushinteger(L, signal);

    return 2;
}

int Wrap_System::getProductInfo(lua_State* L)
{
    auto info = instance()->getProductInfo();

    luax_pushstring(L, info.model);
    luax_pushstring(L, info.version);
    luax_pushstring(L, info.region);

    return 3;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getProcessorCount",   Wrap_System::getProcessorCount   },
    { "getPowerInfo",        Wrap_System::getPowerInfo        },
    { "getClipboardText",    Wrap_System::getClipboardText    },
    { "setClipboardText",    Wrap_System::setClipboardText    },
    { "vibrate",             Wrap_System::vibrate             },
    { "openURL",             Wrap_System::openURL             },
    { "hasBackgroundMusic",  Wrap_System::hasBackgroundMusic  },
    { "getPreferredLocales", Wrap_System::getPreferredLocales },
    { "getNetworkInfo",      Wrap_System::getNetworkInfo      },
    { "getProductInfo",      Wrap_System::getProductInfo      },
    { "getOS",               Wrap_System::getOS               }
};

#if defined(__3DS__)
static constexpr std::array<luaL_Reg, 2> platformFunctions =
{{
    { "getPlayCoins", Wrap_System::getPlayCoins },
    { "setPlayCoins", Wrap_System::setPlayCoins }
}};
#else
static constexpr std::span<const luaL_Reg> platformFunctions = {};
#endif
// clang-format on

int Wrap_System::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new System(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance          = instance;
    module.name              = "system";
    module.type              = &Module::type;
    module.functions         = functions;
    module.platformFunctions = platformFunctions;
    module.types             = {};

    return luax_register_module(L, module);
}
