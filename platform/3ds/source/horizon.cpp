#include "common/runtime.h"
#include "common/backend/horizon.h"

void Horizon::Initialize()
{
    if (initialized)
        return;

    cfguInit();
    ptmuInit();
    acInit();
    mcuHwcInit();

    m_regions =
    {
        { CFG_REGION_AUS, "AUS" },
        { CFG_REGION_CHN, "CHN" },
        { CFG_REGION_EUR, "EUR" },
        { CFG_REGION_JPN, "JPN" },
        { CFG_REGION_KOR, "KOR" },
        { CFG_REGION_TWN, "TWN" },
        { CFG_REGION_USA, "USA" }
    };

    m_languages =
    {
        { CFG_LANGUAGE_DE, "German"              },
        { CFG_LANGUAGE_EN, "English"             },
        { CFG_LANGUAGE_ES, "Spanish"             },
        { CFG_LANGUAGE_FR, "French"              },
        { CFG_LANGUAGE_IT, "Italian"             },
        { CFG_LANGUAGE_JP, "Japanese"            },
        { CFG_LANGUAGE_KO, "Korean"              },
        { CFG_LANGUAGE_NL, "Dutch"               },
        { CFG_LANGUAGE_PT, "Portugese"           },
        { CFG_LANGUAGE_RU, "Russian"             },
        { CFG_LANGUAGE_TW, "Traditional Chinese" },
        { CFG_LANGUAGE_ZH, "Simplified Chinese"  }
    };

    initialized = true;
}

int Horizon::GetProcessorCount()
{
    bool isN3DS = false;
    APT_CheckNew3DS(&isN3DS);

    int count = 2;
    if (isN3DS)
        count = 4;

    return count;
}

std::string Horizon::GetRegion()
{
    u8 region;
    CFGU_SecureInfoGetRegion(&region);

    return m_regions[region];
}

std::string Horizon::GetLanguage()
{
    u8 language;
    CFGU_GetSystemLanguage(&language);

    return m_languages[language];
}

std::pair<std::string, u8> Horizon::GetPowerInfo()
{
    u8 batteryPercent = 100;
    MCUHWC_GetBatteryLevel(&batteryPercent);

    u8 batteryStateBool;
    PTMU_GetBatteryChargeState(&batteryStateBool);

    std::string state = "battery";
    if (batteryStateBool == 1)
        state = "charging";
    else if (batteryStateBool == 1 && batteryPercent == 100)
        state = "charged";

    return std::make_pair(state, batteryPercent);
}

std::string Horizon::GetUsername()
{
    u16 utf16_username[0x1C] = {0};

    CFGU_GetConfigInfoBlk2(0x1C, 0x000A0000, (u8 *)utf16_username);

    ssize_t utf8_len = utf16_to_utf8(NULL, utf16_username, 0);

    std::string username(utf8_len, '\0');

    utf16_to_utf8((uint8_t *)username.data(), utf16_username, utf8_len);

    username[utf8_len] = '\0';

    return username;
}

std::pair<std::string, int> Horizon::GetWiFiStatus()
{
    u8 wifiStrength = osGetWifiStrength();
    u32 status = 0;

    ACU_GetWifiStatus(&status);

    std::string stat = "disconnected";
    if (status != 0)
        stat = "connected";

    return std::make_pair(stat, wifiStrength);
}

void Horizon::Exit()
{
    ptmuExit();
    cfguExit();
    acExit();
    mcuHwcExit();
}
