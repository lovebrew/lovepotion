#include "modules/system/wrap_System.hpp"

using namespace love;

#define instance() Module::getInstance<System>(Module::M_SYSTEM)

int Wrap_System::getOS(lua_State* L)
{
    luax_pushstring(L, System::getOS());

    return 1;
}

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

    if (percent >= 0)
        lua_pushinteger(L, percent);
    else
        lua_pushnil(L);

    if (seconds >= 0)
        lua_pushinteger(L, seconds);
    else
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

int Wrap_System::getNetworkInfo(lua_State* L)
{
    int32_t signal    = 0;
    auto networkState = instance()->getNetworkInfo(signal);

    std::string_view state {};
    if (!System::getConstant(networkState, state))
        state = "unknown";

    luax_pushstring(L, state);

    if (signal >= 0)
        lua_pushinteger(L, signal);
    else
        lua_pushnil(L);

    return 2;
}

int Wrap_System::getInfo(lua_State* L)
{
    System::ProductInfo info {};

    if (instance()->getInfo(info))
    {
        if (lua_istable(L, 1))
            lua_pushvalue(L, 1);
        else
            lua_createtable(L, 0, 3);

        luax_pushstring(L, info.version);
        lua_setfield(L, -2, "version");

        std::string_view model {};
        if (!System::getConstant((SystemModel)info.model, model))
            model = "Unknown";

        luax_pushstring(L, model);
        lua_setfield(L, -2, "model");

        std::string_view region {};
        if (!System::getConstant((SystemRegion)info.region, region))
            region = "Unknown";

        luax_pushstring(L, region);
        lua_setfield(L, -2, "region");
    }
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_System::getFriendInfo(lua_State* L)
{
    System::FriendInfo info {};

    if (instance()->getFriendInfo(info))
    {
        if (lua_istable(L, 1))
            lua_pushvalue(L, 1);
        else
            lua_createtable(L, 0, 2);

        luax_pushstring(L, info.username);
        lua_setfield(L, -2, "username");

        if (!info.friendcode.empty())
            luax_pushstring(L, info.friendcode);
        else
            lua_pushnil(L);

        lua_setfield(L, -2, "friendcode");
    }
    else
        lua_pushnil(L);

    return 1;
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
    { "getInfo",             Wrap_System::getInfo             },
    { "getFriendInfo",       Wrap_System::getFriendInfo       },
    { "getOS",               Wrap_System::getOS               }
};

#if defined(__3DS__)
int Wrap_System::getPlayCoins(lua_State* L)
{
    int coins = 0;

    luax_catchexcept(L, [&]() { coins = instance()->getPlayCoins(); });
    lua_pushinteger(L, coins);

    return 1;
}

int Wrap_System::setPlayCoins(lua_State* L)
{
    int coins = luaL_checkinteger(L, 1);

    luax_catchexcept(L, [&]() { instance()->setPlayCoins(coins); });

    return 0;
}

static constexpr std::array<luaL_Reg, 2> platformFunctions =
{{
    { "getPlayCoins", Wrap_System::getPlayCoins },
    { "setPlayCoins", Wrap_System::setPlayCoins }
}};
#elif defined(__SWITCH__)
int Wrap_System::getTheme(lua_State*L)
{
    ColorSetId colorSet;
    setsysGetColorSetId(&colorSet);

    std::string_view name {};
    if (!System::getConstant(colorSet, name))
        name = "Unknown";

    luax_pushstring(L, name);

    return 1;
}

static constexpr std::array<luaL_Reg, 1> platformFunctions =
{{
    { "getTheme", Wrap_System::getTheme }
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
