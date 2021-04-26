#include "modules/system/system.h"
#include <switch.h>

#include "common/results.h"

using namespace love;

/* https://tinyurl.com/yyh7tnml */
int System::GetProcessorCount()
{
    return TEGRA_CPU_COUNT;
}

love::System::PowerInfo System::GetPowerInfo() const
{
    u32 batteryPercent      = 100;
    ChargerType chargerType = ChargerType_None;

    psmGetBatteryChargePercentage(&batteryPercent);
    psmGetChargerType(&chargerType);

    PowerInfo info;
    info.percentage = batteryPercent;
    info.state      = (chargerType == ChargerType_Charger && batteryPercent == 100)
                     ? "charged"
                     : (chargerType == ChargerType_Charger) ? "charging" : "battery";

    return info;
}

const std::string& System::GetUsername()
{
    if (!this->systemInfo.username.empty())
        return this->systemInfo.username;

    AccountUid userID = { 0 };

    AccountProfile profile;
    AccountProfileBase base;

    memset(&base, 0, sizeof(base));

    /* Check we get a Pre-Selected User */
    R_UNLESS(accountGetPreselectedUser(&userID), LOVE_STRING_EMPTY);

    /* Get the profile */
    R_UNLESS(accountGetProfile(&profile, userID), LOVE_STRING_EMPTY);

    /* Get the base profile */
    R_UNLESS(accountProfileGet(&profile, NULL, &base), LOVE_STRING_EMPTY);

    this->systemInfo.username = base.nickname;

    accountProfileClose(&profile);

    return this->systemInfo.username;
}

love::System::NetworkInfo System::GetNetworkInfo() const
{
    bool enabled;
    nifmIsWirelessCommunicationEnabled(&enabled);

    u32 wifiStrength = 0;
    Result res       = nifmGetInternetConnectionStatus(NULL, &wifiStrength, NULL);

    NetworkInfo info;
    info.signal = wifiStrength;
    info.status = (enabled && !R_FAILED(res)) ? "connected" : "disconnected";

    return info;
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

    int32_t model = 0;

    /* Get the Product Model */
    R_UNLESS(setsysGetProductModel(&model), LOVE_STRING_EMPTY);

    const char* name = nullptr;
    if (!System::GetConstant(static_cast<ProductModel>(model), name))
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

    return LOVE_STRING_EMPTY;
}

const std::string& System::GetSystemTheme()
{
    const char* theme = nullptr;
    ColorSetId colorID;

    R_UNLESS(setsysGetColorSetId(&colorID), LOVE_STRING_EMPTY);

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

bool System::GetConstant(const char* in, ProductModel& out)
{
    return models.Find(in, out);
}

bool System::GetConstant(ProductModel in, const char*& out)
{
    return models.Find(in, out);
}

std::vector<const char*> System::GetConstants(ProductModel)
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

constexpr StringMap<System::ProductModel, System::ProductModel::MODEL_MAX_ENUM>::Entry modelEntries[] =
{
    { "Invalid",           System::ProductModel::MODEL_INVALID },
    { "Erista",            System::ProductModel::MODEL_NX      },
    { "Erista Simulation", System::ProductModel::MODEL_COPPER  },
    { "Mariko",            System::ProductModel::MODEL_IOWA    },
    { "Mariko Lite",       System::ProductModel::MODEL_HOAG    },
    { "Mariko Simulation", System::ProductModel::MODEL_CALCIO  },
    { "Mariko Pro",        System::ProductModel::MODEL_AULA    }
};

constinit const StringMap<System::ProductModel, System::ProductModel::MODEL_MAX_ENUM> System::models(modelEntries);

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
