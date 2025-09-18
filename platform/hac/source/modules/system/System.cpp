#include "common/Result.hpp"

#include "modules/system/System.hpp"

namespace love
{
    System::System()
    {
        if (!ResultCode(accountGetPreselectedUser(&this->account)))
        {
            PselUiSettings settings {};

            pselUiCreate(&settings, PselUiMode_UserSelector);
            pselUiShow(&settings, &this->account);
        }
    }

    int System::getProcessorCount() const
    {
        return 0x04;
    }

    System::PowerState System::getPowerInfo(int& seconds, int& percent) const
    {
        PowerState state = POWER_UNKNOWN;

        seconds = -1;
        percent = -1;

        if (!ResultCode(psmGetBatteryChargePercentage((uint32_t*)&percent)))
            return PowerState::POWER_UNKNOWN;

        PsmChargerType type = PsmChargerType_Unconnected;
        if (!ResultCode(psmGetChargerType(&type)))
            return PowerState::POWER_UNKNOWN;

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

        if (percent == 100 && state == POWER_CHARGING)
            state = POWER_CHARGED;

        return state;
    }

    System::NetworkState System::getNetworkInfo(int32_t& signal) const
    {
        signal = -1;

        if (!ResultCode(nifmGetInternetConnectionStatus(nullptr, (uint32_t*)&signal, nullptr)))
            return NETWORK_UNKNOWN;

        return NETWORK_CONNECTED;
    }

    bool System::getFriendInfo(FriendInfo& info) const
    {
        FriendsUserSetting settings {};
        if (!ResultCode(friendsGetUserSetting(this->account, &settings)))
            return false;

        info.friendcode = settings.friend_code;

        AccountProfile profile {};

        if (!ResultCode(accountGetProfile(&profile, this->account)))
            return false;

        AccountProfileBase profileBase {};

        if (!ResultCode(accountProfileGet(&profile, nullptr, &profileBase)))
            return false;

        info.username = profileBase.nickname;

        return true;
    }

    bool System::getInfo(ProductInfo& info) const
    {
        if (!ResultCode(setsysGetProductModel((SetSysProductModel*)&info.model)))
            return false;

        SetSysFirmwareVersion version {};

        if (!ResultCode(setsysGetFirmwareVersion(&version)))
            return false;

        info.version = version.display_version;

        if (!ResultCode(setGetRegionCode((SetRegion*)&info.region)))
            return false;

        return true;
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

    int System::getMemorySize() const
    {
        uint64_t size = 0;
        svcGetInfo(&size, SystemInfoType_TotalPhysicalMemorySize, 0, PhysicalMemorySystemInfo_System);

        return size / (1024 * 1024);
    }
} // namespace love
