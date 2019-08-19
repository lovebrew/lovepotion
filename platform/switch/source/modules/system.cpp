#include "common/runtime.h"
#include "modules/system.h"

void System::Initialize()
{
    /*
    ** Include things like configuration
    ** Account name that runs the game
    ** Battery info (power, state)
    ** Region and language
    */
    setInitialize();
    accountInitialize();
    plInitialize();
    psmInitialize();
    nifmInitialize();
}

//Löve2D Functions

//love.system.getProcessorCount
int System::GetProcessorCount(lua_State * L)
{
    lua_pushnumber(L, 4);

    return 1;
}

//love.system.getPowerInfo
int System::GetPowerInfo(lua_State * L)
{
    u32 batteryPercent = 0;
    psmGetBatteryChargePercentage(&batteryPercent);

    ChargerType chargerType;
    psmGetChargerType(&chargerType);

    string batteryState = (chargerType == ChargerType_None) ? "battery" : "charging";
    if (batteryPercent == 100 && batteryState == "charging")
        batteryState = "charged";

    lua_pushstring(L, batteryState.c_str());
    lua_pushnumber(L, batteryPercent);
    lua_pushnil(L);

    return 3;
}

//love.system.getRegion
int System::GetRegion(lua_State * L)
{
    SetRegion region;
    setGetRegionCode(&region);

    lua_pushstring(L, REGIONS[region].c_str());

    return 1;
}

//love.system.getLanguage
int System::GetLanguage(lua_State * L)
{
    u64 languageString;
    s32 languageID;

    setGetSystemLanguage(&languageString);
    setMakeLanguage(languageString, &languageID);

    lua_pushstring(L, LANGUAGES[languageID].c_str());

    return 1;
}

//love.system.getInternetStatus
int System::GetInternetStatus(lua_State * L)
{
    u32 strength = 0;
    NifmInternetConnectionStatus status;
    NifmInternetConnectionType connectionType;

    string type = "wireless"

    Result rc = nifmGetInternetConnectionStatus(&connectionType, &strength, &status);

    if (R_SUCCEEDED(rc))
    {
        if (connectionType == NifmInternetConnectionType_Ethernet)
            type = "ethernet";
    }
    else if (R_FAILED(rc))
        type = "airplane";

    lua_pushstring(L, type.c_str());
    lua_pushnumber(L, strength);
    
    bool connected = (status == NifmInternetConnectionStatus_Connected);
    lua_pushboolean(L, connected);

    return 3;
}

//love.system.getUsername
int System::GetUsername(lua_State * L)
{
    u128 userID = 0;
    AccountProfile profile;
    AccountUserData userdata;
    AccountProfileBase profilebase;

    char username[0x21];

    memset(&userdata, 0, sizeof(userdata));
    memset(&profilebase, 0, sizeof(profilebase));

    Result resultCode = accountGetPreselectedUser(&userID);

    if (R_SUCCEEDED(resultCode))
    {
        resultCode = accountGetProfile(&profile, userID);

        if (R_SUCCEEDED(resultCode))
        {
            resultCode = accountProfileGet(&profile, &userdata, &profilebase);

            if (R_SUCCEEDED(resultCode))
            {
                memset(username, 0, sizeof(username));
                strncpy(username, profilebase.username, sizeof(username) - 1);

                lua_pushstring(L, username);

                accountProfileClose(&profile);

                return 1;
            }
        }
    }

    //In the event it fails, close it
    accountProfileClose(&profile);
    lua_pushnil(L);

    return 1;
}

//End Löve2D Functions

void System::Exit()
{
    setExit();
    accountExit();
    plExit();
    psmExit();
    nifmExit();
}
