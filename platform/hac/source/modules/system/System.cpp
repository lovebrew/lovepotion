#include "modules/system/System.hpp"

namespace love
{
    System::System()
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

    int System::getProcessorCount() const
    {
        return 0x04;
    }

    System::PowerState System::getPowerInfo(int& seconds, int& percent) const
    {
        uint32_t currentPercent = 100;
        PsmChargerType type     = PsmChargerType_Unconnected;
        PowerState state        = PowerState::POWER_UNKNOWN;

        psmGetBatteryChargePercentage(&currentPercent);
        psmGetChargerType(&type);

        switch (type)
        {
            case PsmChargerType_Unconnected:
                state = POWER_BATTERY;
                break;
            case PsmChargerType_LowPower:
            case PsmChargerType_EnoughPower:
                state = POWER_CHARGING;
                break;
            default:
                state = POWER_UNKNOWN;
                break;
        }

        if (currentPercent == 100 && state == POWER_CHARGING)
            state = POWER_CHARGED;

        seconds = 0;
        percent = currentPercent;

        return state;
    }

    System::NetworkState System::getNetworkInfo(uint8_t& signal) const
    {
        NetworkState state = NETWORK_UNKNOWN;
        uint32_t strength  = 0;

        const auto result = nifmGetInternetConnectionStatus(nullptr, &strength, nullptr);

        signal = (uint8_t)strength;
        state  = R_SUCCEEDED(result) ? NETWORK_CONNECTED : NETWORK_DISCONNECTED;

        return state;
    }

    System::FriendInfo System::getFriendInfo() const
    {
        FriendInfo info {};

        FriendsUserSetting settings {};
        if (R_SUCCEEDED(friendsGetUserSetting(this->account, &settings)))
            info.friendCode = settings.friend_code;

        AccountProfile profile {};
        AccountProfileBase profileBase {};

        if (R_FAILED(accountGetProfile(&profile, this->account)))
            info.username = "Unknown";
        else
        {
            if (R_SUCCEEDED(accountProfileGet(&profile, nullptr, &profileBase)))
                info.username = profileBase.nickname;
            else
                info.username = "Unknown";
        }

        return info;
    }

    System::ProductInfo System::getProductInfo() const
    {
        ProductInfo info {};

        SetSysProductModel model = SetSysProductModel_Invalid;
        std::string_view modelStr {};

        if (R_FAILED(setsysGetProductModel(&model)))
            modelStr = "Unknown";

        System::getConstant(model, modelStr);

        SetSysFirmwareVersion version {};
        std::string_view versionStr = "Unknown";

        if (R_SUCCEEDED(setsysGetFirmwareVersion(&version)))
            versionStr = version.display_version;

        std::string_view regionStr {};
        SetRegion region = SetRegion_JPN;

        if (R_FAILED(setGetRegionCode(&region)))
            regionStr = "Unknown";

        System::getConstant(region, regionStr);

        return { std::string(modelStr), std::string(versionStr), std::string(regionStr) };
    }

    std::vector<std::string> System::getPreferredLocales() const
    {
        std::vector<std::string> locales {};

        uint64_t languageCode = 0;
        SetLanguage language  = SetLanguage_ENUS;

        std::string_view locale {};
        if (R_FAILED(setGetSystemLanguage(&languageCode)))
            locale = "Unknown";

        if (R_FAILED(setMakeLanguage(languageCode, &language)))
            locale = "Unknown";

        System::getConstant(language, locale);

        locales.push_back(std::string(locale));

        return locales;
    }
} // namespace love
