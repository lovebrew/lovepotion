#include "common/runtime.h"
#include "modules/system.h"

using namespace love::ctr;

static std::string empty;

std::array<std::string, LANGUAGE_COUNT> love::System::LANGUAGES =
{
    "Japanese", "English", "French",
    "German",   "Italian", "Spanish",
    "Simplified Chinese",  "Korean",
    "Dutch",    "Portugese",
    "Russian",  "Traditional Chinese"
};

int System::GetProcessorCount()
{
    if (this->sysInfo.processors != 0)
        return this->sysInfo.processors;

    u8 model;
    CFGU_GetSystemModel(&model);

    int processorCount = 2;
    if (model == 2 || model >= 4)
        processorCount = 4;

    return this->sysInfo.processors = processorCount;
}

love::System::PowerInfo System::GetPowerInfo() const
{
    u8 batteryPercent = 100;
    u8 batteryState = 0;

    MCUHWC_GetBatteryLevel(&batteryPercent);
    PTMU_GetBatteryChargeState(&batteryState);

    PowerInfo info;
    info.percentage = batteryPercent;
    info.state = (batteryState == 1 && batteryPercent == 100) ? "charged" :
                 (batteryState == 1) ? "charging" : "battery";

    return info;
}

const std::string & System::GetUsername()
{
    if (!this->sysInfo.username.empty())
        return this->sysInfo.username;

    u16 utf16_username[USERNAME_LENGTH] = {0};

    CFGU_GetConfigInfoBlk2(USERNAME_LENGTH, 0x000A0000, utf16_username);

    ssize_t utf8_len = utf16_to_utf8(NULL, utf16_username, 0);

    this->sysInfo.username = std::string(utf8_len, '\0');

    utf16_to_utf8((uint8_t *)this->sysInfo.username.data(), utf16_username, utf8_len);

    this->sysInfo.username[utf8_len] = '\0';

    return this->sysInfo.username;
}

love::System::NetworkInfo System::GetNetworkInfo() const
{
    u32 status = 0;
    ACU_GetWifiStatus(&status);

    NetworkInfo info;
    info.signal = osGetWifiStrength();
    info.status = (status > 0) ? "connected" : "disconnected";

    return info;
}

const std::string & System::GetLanguage()
{
    if (!this->sysInfo.language.empty())
        return this->sysInfo.language;

    u8 language;
    CFGU_GetSystemLanguage(&language);

    this->sysInfo.language = love::System::LANGUAGES[language];

    return this->sysInfo.language;
}

const std::string & System::GetVersion()
{
    if (!this->sysInfo.version.empty())
        return this->sysInfo.version;

    char out[256] = { 0 };
    Result res = osGetSystemVersionDataString(NULL, NULL, out, 256);

    if (R_FAILED(res))
        return empty;

    this->sysInfo.version = out;
    return this->sysInfo.version;
}