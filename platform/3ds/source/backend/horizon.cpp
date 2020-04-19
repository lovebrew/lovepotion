#include "common/runtime.h"
#include "backend/horizon.h"

#define USERNAME_LENGTH 0x1C

/* Available System Languages */

std::array<std::string, LANGUAGE_COUNT> Horizon::LANGUAGES =
{
    "Japanese", "English", "French",
    "German", "Italian", "Spanish",
    "Simplified Chinese", "Korean",
    "Dutch", "Portugese", "Russian",
    "Traditional Chinese"
};

/* Implementation for Backend Stuff */

int Horizon::GetCPUCount()
{
    if (Horizon::CPU_COUNT != 0)
        return Horizon::CPU_COUNT;

    u8 model;
    CFGU_GetSystemModel(&model);

    int processorCount = 2;
    if (model == 2 || model >= 4)
        processorCount = 4;

    return Horizon::CPU_COUNT = processorCount;
}

std::string Horizon::GetUsername()
{
    if (!Horizon::USERNAME.empty())
        return Horizon::USERNAME;

    u16 utf16_username[USERNAME_LENGTH] = {0};

    CFGU_GetConfigInfoBlk2(USERNAME_LENGTH, 0x000A0000, (u8 *)utf16_username);

    ssize_t utf8_len = utf16_to_utf8(NULL, utf16_username, 0);

    Horizon::USERNAME = std::string(utf8_len, '\0');

    utf16_to_utf8((uint8_t *)Horizon::USERNAME.data(), utf16_username, utf8_len);

    Horizon::USERNAME[utf8_len] = '\0';

    return Horizon::USERNAME;
}

std::string Horizon::GetSystemLanguage()
{
    if (!Horizon::LANGUAGE.empty())
        return Horizon::LANGUAGE;

    u8 language;
    CFGU_GetSystemLanguage(&language);

    return Horizon::LANGUAGE = LANGUAGES[language];
}

Horizon::PowerInfo Horizon::GetPowerInfo()
{
    u8 batteryPercent, batteryState;

    MCUHWC_GetBatteryLevel(&batteryPercent);
    PTMU_GetBatteryChargeState(&batteryState);

    PowerInfo info;
    info.percentage = batteryPercent;
    info.state = (batteryState == 1 && batteryPercent == 100) ? "charged" :
                    (batteryState == 1) ? "charging" : "battery";

    return info;
}

Horizon::NetworkInfo Horizon::GetNetworkInfo()
{
    u32 status;
    ACU_GetWifiStatus(&status);

    NetworkInfo info;
    info.signal = osGetWifiStrength();
    info.status = (status > 0) ? "connected" : "disconnected";

    return info;
}
