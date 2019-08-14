#include "common/runtime.h"
#include "modules/system.h"

void System::Initialize()
{
    cfguInit();
    ptmuInit();
    acInit();
    mcuHwcInit();
}

//Löve2D Functions

//love.system.getProcessorCount
int System::GetProcessorCount(lua_State * L)
{
    u8 model;
    CFGU_GetSystemModel(&model);

    int processorCount = 2;
    if (model == 2 || model >= 4)
        processorCount = 4;

    lua_pushnumber(L, processorCount);

    return 1;
}

//love.system.getPowerInfo
int System::GetPowerInfo(lua_State * L)
{   
    u8 batteryPercent = 100;

    MCUHWC_GetBatteryLevel(&batteryPercent);
    
    u8 batteryStateBool;
    PTMU_GetBatteryChargeState(&batteryStateBool);
    
    std::string batteryState = "battery";
    if (batteryStateBool == 1)
        batteryState = "charging";
    else if (batteryStateBool == 1 && batteryPercent == 100)
        batteryState = "charged";

    lua_pushstring(L, batteryState.c_str());
    lua_pushnumber(L, batteryPercent);
    lua_pushnil(L);
    
    return 3;
}

//love.system.getRegion
int System::GetRegion(lua_State * L)
{
    u8 region;
    CFGU_SecureInfoGetRegion(&region);

    lua_pushstring(L, REGIONS[region].c_str());

    return 1;
}

//love.system.getLanguage
int System::GetLanguage(lua_State * L)
{
    u8 language;
    CFGU_GetSystemLanguage(&language);

    lua_pushstring(L, LANGUAGES[language].c_str());

    return 1;
}

//love.system.getInternetStatus
int System::GetInternetStatus(lua_State * L)
{
    u8 wifiStrength = osGetWifiStrength();
    u32 status = 0;

    ACU_GetWifiStatus(&status);
    
    lua_pushnumber(L, wifiStrength);
    lua_pushboolean(L, status != 0);

    return 1;
}

//love.system.getUsername
int System::GetUsername(lua_State * L)
{
    u16 utf16_username[0x1C] = {0};

    CFGU_GetConfigInfoBlk2(0x1C, 0x000A0000, (u8 *)utf16_username);
    
    ssize_t utf8_len = utf16_to_utf8(NULL, utf16_username, 0);

    string username(utf8_len, '\0');

    utf16_to_utf8((uint8_t *)username.data(), utf16_username, utf8_len);

    username[utf8_len] = '\0';

    lua_pushstring(L, username.c_str());

    return 1;
}

//End Löve2D Functions

void System::Exit()
{
    ptmuExit();
    cfguExit();
    acExit();
    mcuHwcExit();
}
