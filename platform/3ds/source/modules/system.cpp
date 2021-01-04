#include <3ds.h>
#include "modules/system/system.h"

using namespace love;

static std::string empty;

int System::GetProcessorCount()
{
    if (this->sysInfo.processors != 0)
        return this->sysInfo.processors;

    uint8_t model = 0;
    CFGU_GetSystemModel(&model);

    int processorCount = 2;

    switch (model)
    {
        case CFG_MODEL_N3DS:
        case CFG_MODEL_N3DSXL:
        case CFG_MODEL_N2DSXL:
            processorCount = 4;
            break;
    }

    return this->sysInfo.processors = processorCount;
}

const std::string & System::GetModel()
{
    if (!this->sysInfo.model.empty())
        return this->sysInfo.model;

    uint8_t model = 0;
    CFGU_GetSystemModel(&model);

    /* ideally this shouldn't happen */

    const char * name = nullptr;
    if (!System::GetConstant(static_cast<CFG_SystemModel>(model), name))
        name = "Unknown";

    this->sysInfo.model = name;

    return this->sysInfo.model;
}

System::PowerInfo System::GetPowerInfo() const
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

const std::string & System::GetRegion()
{
    if (!this->sysInfo.region.empty())
        return this->sysInfo.region;

    uint8_t region = 0;
    CFGU_SecureInfoGetRegion(&region);

    /* ideally this shouldn't happen */

    const char * name = nullptr;
    if (!System::GetConstant(static_cast<CFG_Region>(region), name))
        name = "Unknown";

    this->sysInfo.region = name;

    return this->sysInfo.region;
}

System::NetworkInfo System::GetNetworkInfo() const
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

    uint8_t language;
    CFGU_GetSystemLanguage(&language);

    /* ideally this shouldn't happen */

    const char * name = nullptr;
    if (!System::GetConstant(static_cast<CFG_Language>(language), name))
        name = "Unknown";

    this->sysInfo.language = name;

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

/* LANGUAGE CONSTANTS */

bool System::GetConstant(const char * in, CFG_Language & out)
{
    return System::languages.Find(in, out);
}

bool System::GetConstant(CFG_Language in, const char *& out)
{
    return System::languages.Find(in, out);
}

std::vector<std::string> System::GetConstants(CFG_Language)
{
    return System::languages.GetNames();
}

StringMap<CFG_Language, System::MAX_LANGUAGES>::Entry System::languageEntries[] =
{
    { "Japanese",            CFG_LANGUAGE_JP },
    { "English",             CFG_LANGUAGE_EN },
    { "French",              CFG_LANGUAGE_FR },
    { "German",              CFG_LANGUAGE_DE },
    { "Italian",             CFG_LANGUAGE_IT },
    { "Spanish",             CFG_LANGUAGE_ES },
    { "Simplified Chinese",  CFG_LANGUAGE_ZH },
    { "Korean",              CFG_LANGUAGE_KO },
    { "Dutch",               CFG_LANGUAGE_NL },
    { "Portugese",           CFG_LANGUAGE_PT },
    { "Russian",             CFG_LANGUAGE_RU },
    { "Traditional Chinese", CFG_LANGUAGE_TW }
};

StringMap<CFG_Language, System::MAX_LANGUAGES> System::languages(System::languageEntries, sizeof(System::languageEntries));

/* MODEL CONSTANTS */

bool System::GetConstant(const char * in, CFG_SystemModel & out)
{
    return System::models.Find(in, out);
}

bool System::GetConstant(CFG_SystemModel in, const char *& out)
{
    return System::models.Find(in, out);
}

std::vector<std::string> System::GetConstants(CFG_SystemModel)
{
    return System::models.GetNames();
}

StringMap<CFG_SystemModel, System::MAX_MODELS>::Entry System::modelEntries[] =
{
    { "3DS",       CFG_MODEL_3DS    },
    { "3DSXL",     CFG_MODEL_3DSXL  },
    { "New 3DS",   CFG_MODEL_N3DS   },
    { "2DS",       CFG_MODEL_2DS    },
    { "New 3DSXL", CFG_MODEL_N3DSXL },
    { "New 2DSXL", CFG_MODEL_N2DSXL },
};

StringMap<CFG_SystemModel, System::MAX_MODELS> System::models(System::modelEntries, sizeof(System::modelEntries));

/* REGION CONSTANTS */

bool System::GetConstant(const char * in, CFG_Region & out)
{
    return System::regions.Find(in, out);
}

bool System::GetConstant(CFG_Region in, const char *& out)
{
    return System::regions.Find(in, out);
}

std::vector<std::string> System::GetConstants(CFG_Region)
{
    return System::regions.GetNames();
}

StringMap<CFG_Region, System::MAX_REGIONS>::Entry System::regionEntries[] =
{
    { "Japan",         CFG_REGION_JPN },
    { "United States", CFG_REGION_USA },
    { "Europe",        CFG_REGION_EUR },
    { "Australia",     CFG_REGION_AUS },
    { "China",         CFG_REGION_CHN },
    { "Korea",         CFG_REGION_KOR },
    { "Taiwan",        CFG_REGION_TWN },
};

StringMap<CFG_Region, System::MAX_REGIONS> System::regions(System::regionEntries, sizeof(System::regionEntries));
