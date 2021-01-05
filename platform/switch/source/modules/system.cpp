#include <switch.h>
#include "modules/system/system.h"

using namespace love;

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

static std::string empty;

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
        return empty;

    res = accountGetProfile(&profile, userID);

    if (R_FAILED(res))
        return empty;

    res = accountProfileGet(&profile, NULL, &base);

    if (R_FAILED(res))
        return empty;

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

    uint64_t languageCode;
    SetLanguage language;

    Result res = setGetSystemLanguage(&languageCode);

    if (R_FAILED(res))
        return empty;

    res = setMakeLanguage(languageCode, &language);

    if (R_FAILED(res))
        return empty;

    const char * name = nullptr;
    if (!System::GetConstant(language, name))
        name = "Unknown";

    this->sysInfo.language = name;

    return this->sysInfo.language;
}

const std::string & System::GetModel()
{
    if (!this->sysInfo.model.empty())
        return this->sysInfo.model;

    int32_t model = 0;
    Result res = setsysGetProductModel(&model);

    if (R_FAILED(res))
        return empty;

    const char * name = nullptr;
    if (!System::GetConstant(static_cast<ProductModel>(model), name))
        name = "Unknown";

    this->sysInfo.model = name;

    return this->sysInfo.model;
}

const std::string & System::GetRegion()
{
    if (!this->sysInfo.region.empty())
        return this->sysInfo.region;

    SetRegion region;
    Result res = setGetRegionCode(&region);

    if (R_FAILED(res))
        return empty;

    const char * name = nullptr;
    if (!System::GetConstant(region, name))
        name = "Unknown";

    this->sysInfo.region = name;

    return this->sysInfo.region;
}

const std::string & System::GetVersion()
{
    if (!this->sysInfo.version.empty())
        return this->sysInfo.version;

    SetSysFirmwareVersion firmwareVersion;
    Result res = setsysGetFirmwareVersion(&firmwareVersion);

    if (R_FAILED(res))
        return empty;

    this->sysInfo.version = firmwareVersion.display_version;
    return this->sysInfo.version;
}

/* LANGUAGE CONSTANTS */

bool System::GetConstant(const char * in, SetLanguage & out)
{
    return System::languages.Find(in, out);
}

bool System::GetConstant(SetLanguage in, const char *& out)
{
    return System::languages.Find(in, out);
}

std::vector<std::string> System::GetConstants(SetLanguage)
{
    return System::languages.GetNames();
}

StringMap<SetLanguage, SetLanguage_Total>::Entry System::languageEntries[] =
{
    { "Japanese",               SetLanguage_JA     },
    { "US English",             SetLanguage_ENUS   },
    { "French",                 SetLanguage_FR     },
    { "German",                 SetLanguage_DE     },
    { "Italian",                SetLanguage_IT     },
    { "Spanish",                SetLanguage_ES     },
    { "Chinese",                SetLanguage_ZHCN   },
    { "Korean",                 SetLanguage_KO     },
    { "Dutch",                  SetLanguage_NL     },
    { "Portuguese",             SetLanguage_PT     },
    { "Russian",                SetLanguage_RU     },
    { "Taiwanese",              SetLanguage_ZHTW   },
    { "British English",        SetLanguage_ENGB   },
    { "Canadian French",        SetLanguage_FRCA   },
    { "Latin American Spanish", SetLanguage_ES419  },
    { "Chinese Simplified",     SetLanguage_ZHHANS },
    { "Chinese Traditional",    SetLanguage_ZHHANT }
};

StringMap<SetLanguage, SetLanguage_Total> System::languages(System::languageEntries, sizeof(System::languageEntries));

/* MODEL CONSTANTS */

bool System::GetConstant(const char * in, ProductModel & out)
{
    return System::models.Find(in, out);
}

bool System::GetConstant(ProductModel in, const char *& out)
{
    return System::models.Find(in, out);
}

std::vector<std::string> System::GetConstants(ProductModel)
{
    return System::models.GetNames();
}

StringMap<System::ProductModel, System::ProductModel::MODEL_MAX_ENUM>::Entry System::modelEntries[] =
{
    { "Invalid",           MODEL_INVALID },
    { "Erista",            MODEL_NX      },
    { "Erista Simulation", MODEL_COPPER  },
    { "Mariko",            MODEL_IOWA    },
    { "Mariko Lite",       MODEL_HOAG    },
    { "Mariko Simulation", MODEL_CALCIO  },
    { "Mariko Pro",        MODEL_AULA    }
};

StringMap<System::ProductModel, System::ProductModel::MODEL_MAX_ENUM> System::models(System::modelEntries, sizeof(System::modelEntries));

/* REGION CONSTANTS */

bool System::GetConstant(const char * in, SetRegion & out)
{
    return System::regions.Find(in, out);
}

bool System::GetConstant(SetRegion in, const char *& out)
{
    return System::regions.Find(in, out);
}

std::vector<std::string> System::GetConstants(SetRegion)
{
    return System::regions.GetNames();
}

StringMap<SetRegion, System::MAX_REGIONS>::Entry System::regionEntries[] =
{
    { "Japan",                  SetRegion_JPN },
    { "United States",          SetRegion_USA },
    { "Europe",                 SetRegion_EUR },
    { "Australia/New Zealand",  SetRegion_AUS },
    { "Hong Kong/Taiwan/Korea", SetRegion_HTK },
    { "China",                  SetRegion_CHN }
};

StringMap<SetRegion, System::MAX_REGIONS> System::regions(System::regionEntries, sizeof(System::regionEntries));