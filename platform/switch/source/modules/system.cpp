#include "modules/system/system.h"
#include <switch.h>

#include "common/results.h"

using namespace love;

System::System()
{
    /* Check if there's a pre-selected user first */
    if (R_FAILED(accountGetPreselectedUser(&this->userID)))
    {
        /* Create player selection UI settings */
        PselUiSettings settings;
        pselUiCreate(&settings, PselUiMode_UserSelector);

        /* Ask for a user account */
        pselUiShow(&settings, &this->userID);
    }
}

/* https://tinyurl.com/yyh7tnml */
int System::GetProcessorCount()
{
    return TEGRA_CPU_COUNT;
}

const std::string& System::GetUsername()
{
    if (!this->systemInfo.username.empty())
        return this->systemInfo.username;

    AccountProfile profile {};
    AccountProfileBase base {};

    /* Get the profile from the System's userID we selected */
    R_UNLESS(accountGetProfile(&profile, this->userID), LOVE_STRING_EMPTY);

    /* Get the base profile */
    R_UNLESS(accountProfileGet(&profile, NULL, &base), LOVE_STRING_EMPTY);

    this->systemInfo.username = base.nickname;

    accountProfileClose(&profile);

    return this->systemInfo.username;
}

System::PowerState System::GetPowerInfo(uint8_t& percent) const
{
    uint32_t batteryPercent = 100;
    PsmChargerType type     = PsmChargerType_Unconnected;

    PowerState state = PowerState::POWER_UNKNOWN;

    psmGetBatteryChargePercentage(&batteryPercent);
    psmGetChargerType(&type);

    state = (type > 0 && type != PsmChargerType_NotSupported) ? PowerState::POWER_CHARGING
                                                              : PowerState::POWER_BATTERY;

    if (percent == 100 && type != PsmChargerType_Unconnected)
        state = PowerState::POWER_CHARGED;

    return state;
}

System::NetworkState System::GetNetworkInfo(uint8_t& signal) const
{
    NetworkState state = NetworkState::NETWORK_UNKNOWN;

    uint32_t wifiStrength = 0;
    Result res            = nifmGetInternetConnectionStatus(NULL, &wifiStrength, NULL);

    signal = static_cast<uint8_t>(wifiStrength);
    state = R_SUCCEEDED(res) ? NetworkState::NETWORK_CONNECTED : NetworkState::NETWORK_DISCONNECTED;

    return state;
}

const std::string& System::GetLanguage()
{
    if (!this->systemInfo.language.empty())
        return this->systemInfo.language;

    uint64_t languageCode = 0;
    SetLanguage language;

    /* Get the System Language Code */
    R_UNLESS(setGetSystemLanguage(&languageCode), LOVE_STRING_EMPTY);

    /* Convert the Language Code to SetLanguage */
    R_UNLESS(setMakeLanguage(languageCode, &language), LOVE_STRING_EMPTY);

    const char* name = nullptr;
    if (!System::GetConstant(language, name))
        name = "Unknown";

    this->systemInfo.language = name;

    return this->systemInfo.language;
}

const std::string& System::GetModel()
{
    if (!this->systemInfo.model.empty())
        return this->systemInfo.model;

    SetSysProductModel model = SetSysProductModel_Invalid;

    /* Get the Product Model */
    R_UNLESS(setsysGetProductModel(&model), LOVE_STRING_EMPTY);

    const char* name = nullptr;
    if (!System::GetConstant(model, name))
        name = "Unknown";

    this->systemInfo.model = name;

    return this->systemInfo.model;
}

const std::string& System::GetRegion()
{
    if (!this->systemInfo.region.empty())
        return this->systemInfo.region;

    SetRegion region;

    /* Get the System Region */
    R_UNLESS(setGetRegionCode(&region), LOVE_STRING_EMPTY);

    const char* name = nullptr;
    if (!System::GetConstant(region, name))
        name = "Unknown";

    this->systemInfo.region = name;

    return this->systemInfo.region;
}

const std::string& System::GetVersion()
{
    if (!this->systemInfo.version.empty())
        return this->systemInfo.version;

    SetSysFirmwareVersion firmwareVersion;

    /* Get the System Firmware Version */
    R_UNLESS(setsysGetFirmwareVersion(&firmwareVersion), LOVE_STRING_EMPTY);

    this->systemInfo.version = firmwareVersion.display_version;
    return this->systemInfo.version;
}

const std::string& System::GetFriendCode()
{
    if (!this->systemInfo.friendCode.empty())
        return this->systemInfo.friendCode;

    FriendsUserSetting settings;

    R_UNLESS(friendsGetUserSetting(this->userID, &settings), LOVE_STRING_EMPTY);

    this->systemInfo.friendCode = settings.friend_code;

    return this->systemInfo.friendCode;
}

const std::string& System::GetSystemTheme()
{
    const char* theme = nullptr;
    ColorSetId colorID;

    R_UNLESS(setsysGetColorSetId(&colorID), LOVE_STRING_EMPTY);

    if (!System::GetConstant(colorID, theme))
        theme = "Unknown";

    this->systemInfo.colorTheme = theme;
    return this->systemInfo.colorTheme;
}

/* THEME CONSTANTS */
bool System::GetConstant(const char* in, ColorSetId& out)
{
    return themes.Find(in, out);
}

bool System::GetConstant(ColorSetId in, const char*& out)
{
    return themes.Find(in, out);
}

/* LANGUAGE CONSTANTS */

bool System::GetConstant(const char* in, SetLanguage& out)
{
    return languages.Find(in, out);
}

bool System::GetConstant(SetLanguage in, const char*& out)
{
    return languages.Find(in, out);
}

std::vector<const char*> System::GetConstants(SetLanguage)
{
    return languages.GetNames();
}

/* MODEL CONSTANTS */

bool System::GetConstant(const char* in, SetSysProductModel& out)
{
    return models.Find(in, out);
}

bool System::GetConstant(SetSysProductModel in, const char*& out)
{
    return models.Find(in, out);
}

std::vector<const char*> System::GetConstants(SetSysProductModel)
{
    return models.GetNames();
}

/* REGION CONSTANTS */

bool System::GetConstant(const char* in, SetRegion& out)
{
    return System::regions.Find(in, out);
}

bool System::GetConstant(SetRegion in, const char*& out)
{
    return System::regions.Find(in, out);
}

std::vector<const char*> System::GetConstants(SetRegion)
{
    return System::regions.GetNames();
}

// clang-format off
constexpr StringMap<SetLanguage, SetLanguage_Total>::Entry languageEntries[] =
{
    {  "Japanese",               SetLanguage_JA     },
    {  "US English",             SetLanguage_ENUS   },
    {  "French",                 SetLanguage_FR     },
    {  "German",                 SetLanguage_DE     },
    {  "Italian",                SetLanguage_IT     },
    {  "Spanish",                SetLanguage_ES     },
    {  "Chinese",                SetLanguage_ZHCN   },
    {  "Korean",                 SetLanguage_KO     },
    {  "Dutch",                  SetLanguage_NL     },
    {  "Portuguese",             SetLanguage_PT     },
    {  "Russian",                SetLanguage_RU     },
    {  "Taiwanese",              SetLanguage_ZHTW   },
    {  "British English",        SetLanguage_ENGB   },
    {  "Canadian French",        SetLanguage_FRCA   },
    {  "Latin American Spanish", SetLanguage_ES419  },
    {  "Chinese Simplified",     SetLanguage_ZHHANS },
    {  "Chinese Traditional",    SetLanguage_ZHHANT },
    {  "Brazilian Protuguese",   SetLanguage_PTBR   }
};

constinit const StringMap<SetLanguage, SetLanguage_Total> System::languages(languageEntries);

constexpr StringMap<SetSysProductModel, System::MAX_MODELS>::Entry modelEntries[] =
{
    { "Invalid",           SetSysProductModel_Invalid},
    { "Erista",            SetSysProductModel_Nx      },
    { "Erista Simulation", SetSysProductModel_Copper  },
    { "Mariko",            SetSysProductModel_Iowa    },
    { "Mariko Lite",       SetSysProductModel_Hoag    },
    { "Mariko Simulation", SetSysProductModel_Calcio  },
    { "Mariko Pro",        SetSysProductModel_Aula    }
};

constinit const StringMap<SetSysProductModel, System::MAX_MODELS> System::models(modelEntries);

constexpr StringMap<SetRegion, System::MAX_REGIONS>::Entry regionEntries[] =
{
    { "Japan",                  SetRegion_JPN },
    { "United States",          SetRegion_USA },
    { "Europe",                 SetRegion_EUR },
    { "Australia/New Zealand",  SetRegion_AUS },
    { "Hong Kong/Taiwan/Korea", SetRegion_HTK },
    { "China",                  SetRegion_CHN }
};

constinit const StringMap<SetRegion, System::MAX_REGIONS> System::regions(regionEntries);

constexpr StringMap<ColorSetId, System::MAX_THEMES>::Entry themeEntries[] =
{
    { "dark",  ColorSetId_Dark  },
    { "light", ColorSetId_Light }
};

constinit const StringMap<ColorSetId, System::MAX_THEMES> System::themes(themeEntries);
// clang-format on
