#include <modules/system_ext.hpp>

#include <utilities/result.hpp>

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

    ColorSetId colorID;

    R_UNLESS(setsysGetColorSetId(&colorID), std::string {});

    if (auto thm = System::themes.ReverseFind(colorID))
        this->info.colorTheme = *thm;
    else
        this->info.colorTheme = "Unknown";

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

    if (auto found = System::languages.ReverseFind(language))
        this->info.locale = *found;
    else
        this->info.locale = "Unknown";

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

    if (auto name = System::models.ReverseFind(model))
        this->info.model = *name;
    else
        this->info.model = "Unknown";

    return this->info.model;
}

std::string_view System<Console::HAC>::GetFriendInfo()
{
    if (!this->info.friendCode.empty())
        return this->info.friendCode;

    FriendsUserSetting settings;

    R_UNLESS(friendsGetUserSetting(this->account, &settings), std::string {});

    this->info.friendCode = settings.friend_code;

    return this->info.friendCode;
}

std::string_view System<Console::HAC>::GetUsername()
{
    if (!this->info.username.empty())
        return this->info.username;

    AccountProfile profile {};
    AccountProfileBase base {};

    /* Get the profile from the System's userID we selected */
    R_UNLESS(accountGetProfile(&profile, this->account), std::string {});

    /* Get the base profile */
    R_UNLESS(accountProfileGet(&profile, NULL, &base), std::string {});

    this->info.username = base.nickname;

    accountProfileClose(&profile);

    return this->info.username;
}
