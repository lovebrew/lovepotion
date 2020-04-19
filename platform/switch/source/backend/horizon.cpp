#include "common/runtime.h"
#include "common/backend/horizon.h"

#define USERNAME_LENGTH 0x21

std::array<std::string, LANGUAGE_COUNT> Horizon::LANGUAGES =
{
    "Japanese", "American English", "French",
    "German", "Italian", "Spanish",
    "Chinese", "Korean",
    "Dutch", "Portugese", "Russian",
    "Taiwanese", "British English",
    "Canadian French", "Latin American Spanish",
    "Chinese Simplified", "Chinese Traditional"
};

/* https://xkcd.com/221/ */
int Horizon::GetCPUCount()
{
    return 4;
}

std::string Horizon::GetUsername()
{
    if (!Horizon::USERNAME.empty())
        return Horizon::USERNAME;

    Result res = 0;
    AccountUid userID = { 0 };

    AccountProfile profile;
    AccountProfileBase base;

    memset(&base, 0, sizeof(base));

    res = accountGetPreselectedUser(&userID);

    if (R_FAILED(res))
        return Horizon::USERNAME = "";

    res = accountGetProfile(&profile, userID);

    if (R_FAILED(res))
        return Horizon::USERNAME = "";

    res = accountProfileGet(&profile, NULL, &base);

    if (R_FAILED(res))
        return Horizon::USERNAME = "";

    Horizon::USERNAME = base.nickname;

    accountProfileClose(&profile);

    return Horizon::USERNAME;
}

std::string Horizon::GetSystemLanguage()
{
    if (!Horizon::LANGUAGE.empty())
        return Horizon::LANGUAGE;

    u64 languageCode;
    SetLanguage language;

    Result res = setGetSystemLanguage(&languageCode);

    if (R_FAILED(res))
        return Horizon::LANGUAGE = "";

    res = setMakeLanguage(languageCode, &language);

    if (R_FAILED(res))
        return Horizon::LANGUAGE = "";

    return Horizon::LANGUAGE = LANGUAGES[language];
}

Horizon::PowerInfo Horizon::GetPowerInfo()
{
    u32 batteryPercent;
    ChargerType chargerType;

    psmGetBatteryChargePercentage(&batteryPercent);
    psmGetChargerType(&chargerType);

    PowerInfo info;
    info.percentage = batteryPercent;
    info.state = (chargerType == ChargerType_Charger && batteryPercent == 100) ? "charged" :
                    (chargerType == ChargerType_Charger) ? "charging" : "battery";

    return info;
}

Horizon::NetworkInfo Horizon::GetNetworkInfo()
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
