#include <modules/system_ext.hpp>

#include <utilities/results.hpp>

using namespace love;

System<Console::HAC>::System()
{
    /* Check if there's a pre-selected user first */
    if (R_FAILED(accountGetPreselectedUser(&this->account)))
    {
        /* Create player selection UI settings */
        PselUiSettings settings;
        pselUiCreate(&settings, PselUiMode_UserSelector);

        /* Ask for a user account */
        pselUiShow(&settings, &this->account);
    }
}

System<>::PowerState System<Console::HAC>::GetPowerInfo(uint8_t& percent) const
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

System<>::NetworkState System<Console::HAC>::GetNetworkInfo(uint8_t& signal) const
{
    NetworkState state = NetworkState::NETWORK_UNKNOWN;

    uint32_t wifiStrength = 0;
    Result res            = nifmGetInternetConnectionStatus(NULL, &wifiStrength, NULL);

    signal = static_cast<uint8_t>(wifiStrength);
    state = R_SUCCEEDED(res) ? NetworkState::NETWORK_CONNECTED : NetworkState::NETWORK_DISCONNECTED;

    return state;
}

int System<Console::HAC>::GetProcessorCount()
{
    return 0x04;
}

std::string_view System<Console::HAC>::GetSystemTheme()
{
    if (!this->info.colorTheme.empty())
        return this->info.colorTheme;

    const char* theme = nullptr;
    ColorSetId colorID;

    R_UNLESS(setsysGetColorSetId(&colorID), std::string {});

    if (!System::GetConstant(colorID, theme))
        theme = "Unknown";

    this->info.colorTheme = theme;
    return this->info.colorTheme;
}

std::string_view System<Console::HAC>::GetPreferredLocales()
{
    if (!this->info.locale.empty())
        return this->info.locale;

    uint64_t languageCode = 0;
    SetLanguage language;

    /* Get the System Language Code */
    R_UNLESS(setGetSystemLanguage(&languageCode), std::string {});

    /* Convert the Language Code to SetLanguage */
    R_UNLESS(setMakeLanguage(languageCode, &language), std::string {});

    const char* name = nullptr;
    if (!System::GetConstant(language, name))
        name = "Unknown";

    this->info.locale = name;

    return this->info.locale;
}

std::string_view System<Console::HAC>::GetVersion()
{
    if (!this->info.version.empty())
        return this->info.version;

    SetSysFirmwareVersion firmwareVersion;

    /* Get the System Firmware Version */
    R_UNLESS(setsysGetFirmwareVersion(&firmwareVersion), std::string {});

    this->info.version = firmwareVersion.display_version;

    return this->info.version;
}

std::string_view System<Console::HAC>::GetModel()
{
    if (!this->info.model.empty())
        return this->info.model;

    SetSysProductModel model = SetSysProductModel_Invalid;

    /* Get the Product Model */
    R_UNLESS(setsysGetProductModel(&model), std::string {});

    const char* name = nullptr;
    if (!System<Console::HAC>::GetConstant(model, name))
        name = "Unknown";

    this->info.model = name;

    return name;
}

std::string_view System<Console::HAC>::GetFriendCode()
{
    if (!this->info.friendCode.empty())
        return this->info.friendCode;

    FriendsUserSetting settings;

    R_UNLESS(friendsGetUserSetting(this->account, &settings), std::string {});

    this->info.friendCode = settings.friend_code;

    return this->info.friendCode;
}

// clang-format off
constexpr auto languages = BidirectionalMap<>::Create(
    "jp",      SetLanguage_JA,
    "en_US",   SetLanguage_ENUS,
    "fr",      SetLanguage_FR,
    "de",      SetLanguage_DE,
    "it",      SetLanguage_IT,
    "es",      SetLanguage_ES,
    "zh_CN",   SetLanguage_ZHCN,
    "ko",      SetLanguage_KO,
    "nl",      SetLanguage_NL,
    "pt",      SetLanguage_PT,
    "ru",      SetLanguage_RU,
    "zh_TW",   SetLanguage_ZHTW,
    "en_GB",   SetLanguage_ENGB,
    "fr_CA",   SetLanguage_FRCA,
    "es_419",  SetLanguage_ES419,
    "zh_HANS", SetLanguage_ZHHANS,
    "zh_HANT", SetLanguage_ZHHANT,
    "pt_BR",   SetLanguage_PTBR
);

constexpr auto models = BidirectionalMap<>::Create(
    "Invalid",           SetSysProductModel_Invalid,
    "Erista",            SetSysProductModel_Nx,
    "Erista Simulation", SetSysProductModel_Copper,
    "Mariko",            SetSysProductModel_Iowa,
    "Mariko Lite",       SetSysProductModel_Hoag,
    "Mariko Simulation", SetSysProductModel_Calcio,
    "Mariko Pro",        SetSysProductModel_Aula
);

constexpr auto themes = BidirectionalMap<>::Create(
    "dark",  ColorSetId_Dark,
    "light", ColorSetId_Light
);
// clang-format on

/* THEME CONSTANTS */
bool System<Console::HAC>::GetConstant(const char* in, ColorSetId& out)
{
    return themes.Find(in, out);
}

bool System<Console::HAC>::GetConstant(ColorSetId in, const char*& out)
{
    return themes.ReverseFind(in, out);
}

/* LANGUAGE CONSTANTS */

bool System<Console::HAC>::GetConstant(const char* in, SetLanguage& out)
{
    return languages.Find(in, out);
}

bool System<Console::HAC>::GetConstant(SetLanguage in, const char*& out)
{
    return languages.ReverseFind(in, out);
}

std::vector<const char*> System<Console::HAC>::GetConstants(SetLanguage)
{
    return languages.GetNames();
}

/* MODEL CONSTANTS */

bool System<Console::HAC>::GetConstant(const char* in, SetSysProductModel& out)
{
    return models.Find(in, out);
}

bool System<Console::HAC>::GetConstant(SetSysProductModel in, const char*& out)
{
    return models.ReverseFind(in, out);
}

std::vector<const char*> System<Console::HAC>::GetConstants(SetSysProductModel)
{
    return models.GetNames();
}
