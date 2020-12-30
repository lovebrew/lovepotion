#include <switch.h>
#include "modules/system/system.h"

using namespace love;

std::array<std::string, LANGUAGE_COUNT> love::common::System::LANGUAGES =
{
    "Japanese",  "American English", "French",
    "German",    "Italian",          "Spanish",
    "Chinese",   "Korean",           "Dutch",
    "Portugese", "Russian",          "Taiwanese",
    "British English",               "Canadian French",
    "Latin American Spanish",        "Chinese Simplified",
    "Chinese Traditional"
};

/* https://tinyurl.com/yyh7tnml */
int System::GetProcessorCount()
{
    return TEGRA_CPU_COUNT;
}

love::System::PowerInfo System::GetPowerInfo() const
{
    u32 batteryPercent = 100;
    ChargerType chargerType = ChargerType_None;

    psmGetBatteryChargePercentage(&batteryPercent);
    psmGetChargerType(&chargerType);

    PowerInfo info;
    info.percentage = batteryPercent;
    info.state = (chargerType == ChargerType_Charger && batteryPercent == 100) ? "charged" :
                 (chargerType == ChargerType_Charger) ? "charging" : "battery";

    return info;
}

const std::string & System::GetUsername()
{
    if (!this->sysInfo.username.empty())
        return this->sysInfo.username;

    Result res = 0;
    AccountUid userID = { 0 };

    AccountProfile profile;
    AccountProfileBase base;

    memset(&base, 0, sizeof(base));

    res = accountGetPreselectedUser(&userID);

    if (R_FAILED(res))
        return std::string();

    res = accountGetProfile(&profile, userID);

    if (R_FAILED(res))
        return std::string();

    res = accountProfileGet(&profile, NULL, &base);

    if (R_FAILED(res))
        return std::string();

    this->sysInfo.username = base.nickname;

    accountProfileClose(&profile);

    return this->sysInfo.username;
}

love::System::NetworkInfo System::GetNetworkInfo() const
{
    bool enabled;
    nifmIsWirelessCommunicationEnabled(&enabled);

    u32 wifiStrength;
    Result res = nifmGetInternetConnectionStatus(NULL, &wifiStrength, NULL);

    NetworkInfo info;
    info.signal = wifiStrength;
    info.status = (enabled && !R_FAILED(res)) ? "connected" : "disconnected";

    return info;
}

const std::string & System::GetLanguage()
{
    if (!this->sysInfo.language.empty())
        return this->sysInfo.language;

    u64 languageCode;
    SetLanguage language;

    Result res = setGetSystemLanguage(&languageCode);

    if (R_FAILED(res))
        return std::string();

    res = setMakeLanguage(languageCode, &language);

    if (R_FAILED(res))
        return std::string();

    this->sysInfo.language = LANGUAGES[language];

    return this->sysInfo.language;
}

const std::string & System::GetVersion()
{
    if (!this->sysInfo.version.empty())
        return this->sysInfo.version;

    SetSysFirmwareVersion firmwareVersion;
    Result res = setsysGetFirmwareVersion(&firmwareVersion);

    if (R_FAILED(res))
        return std::string();

    this->sysInfo.version = firmwareVersion.display_version;
    return this->sysInfo.version;
}