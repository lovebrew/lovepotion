#include "modules/system/wrap_system.h"

#include "modules/system/system.h"

using namespace love;

#define instance() (Module::GetInstance<System>(Module::M_SYSTEM))

int Wrap_System::GetOS(lua_State* L)
{
    Luax::PushString(L, instance()->GetOS());

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
    const char* str = nullptr;

    System::PowerState state = instance()->GetPowerInfo(percent);

    if (!common::System::GetConstant(state, str))
        str = "unknown";

    lua_pushstring(L, str);

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
    const char* str = nullptr;

    System::NetworkState state = instance()->GetNetworkInfo(signal);

    if (!common::System::GetConstant(state, str))
        str = "unknown";

    lua_pushstring(L, str);
    lua_pushinteger(L, signal);

    return 2;
}

int Wrap_System::GetLanguage(lua_State* L)
{
    std::string language = instance()->GetLanguage();

    Luax::PushString(L, language);

    return 1;
}

int Wrap_System::GetModel(lua_State* L)
{
    std::string model = instance()->GetModel();

    Luax::PushString(L, model);

    return 1;
}

int Wrap_System::GetRegion(lua_State* L)
{
    std::string region = instance()->GetRegion();

    Luax::PushString(L, region);

    return 1;
}

int Wrap_System::GetUsername(lua_State* L)
{
    std::string username = instance()->GetUsername();

    Luax::PushString(L, username);

    return 1;
}

int Wrap_System::GetVersion(lua_State* L)
{
    std::string version = instance()->GetVersion();

    Luax::PushString(L, version);

    return 1;
}

int Wrap_System::GetFriendCode(lua_State* L)
{
    std::string friendCode = instance()->GetFriendCode();

    Luax::PushString(L, friendCode);

    return 1;
}

int Wrap_System::GetSystemTheme(lua_State* L)
{
    std::string theme = instance()->GetSystemTheme();

    Luax::PushString(L, theme);

    return 1;
}

int Wrap_System::SetPlayCoins(lua_State* L)
{
#if defined(_3DS)
    int amount = luaL_checknumber(L, 1);

    Luax::CatchException(L, [&]() { instance()->SetPlayCoins(amount); });
#endif
    return 0;
}

int Wrap_System::GetPlayCoins(lua_State* L)
{
#if defined(_3DS)
    int amount = 0;

    Luax::CatchException(L, [&]() { amount = instance()->GetPlayCoins(); });

    lua_pushnumber(L, amount);

    return 1;
#endif
    return 0;
}

int Wrap_System::Register(lua_State* L)
{
    luaL_Reg funcs[] = { { "getColorTheme", GetSystemTheme },
                         { "getFriendCode", GetFriendCode },
                         { "getOS", GetOS },
                         { "getProcessorCount", GetProcessorCount },
                         { "getPowerInfo", GetPowerInfo },
                         { "getNetworkInfo", GetNetworkInfo },
                         { "getLanguage", GetLanguage },
                         { "getModel", GetModel },
                         { "getRegion", GetRegion },
                         { "getUsername", GetUsername },
                         { "getVersion", GetVersion },
                         { 0, 0 } };

    /* 3DS extensions */

    luaL_Reg modExt[] = { { "getPlayCoins", GetPlayCoins }, { "setPlayCoins", SetPlayCoins } };

    std::unique_ptr<luaL_Reg[]> reg = Luax::ExtendIf("3DS", funcs, modExt);

    System* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new System(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "system";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = reg.get();
    wrappedModule.types     = nullptr;

    return Luax::RegisterModule(L, wrappedModule);
}
