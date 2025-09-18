#include "common/Result.hpp"

#include "modules/system/System.hpp"

#include <utf8.h>

namespace love
{
    int System::getProcessorCount() const
    {
        bool isNew3DS = false;
        APT_CheckNew3DS(&isNew3DS);

        return isNew3DS ? 4 : 2;
    }

    System::PowerState System::getPowerInfo(int& seconds, int& percent) const
    {
        uint8_t batteryState = 0;
        uint8_t level        = 0;

        seconds = -1;
        percent = -1;

        if (!ResultCode(MCUHWC_GetBatteryLevel(&level)))
            return PowerState::POWER_UNKNOWN;

        percent = (level / 0xFF) * 100;
        if (!ResultCode(PTMU_GetBatteryChargeState(&batteryState)))
            return PowerState::POWER_UNKNOWN;

        auto state = (batteryState) ? PowerState::POWER_CHARGING : PowerState::POWER_BATTERY;

        if (percent == 100 && !batteryState)
            state = PowerState::POWER_CHARGED;

        seconds = 0;

        return state;
    }

    System::NetworkState System::getNetworkInfo(int32_t& signal) const
    {
        uint32_t status = 0;
        signal          = -1;

        if (!ResultCode(ACU_GetWifiStatus(&status)))
            return NetworkState::NETWORK_UNKNOWN;

        signal = osGetWifiStrength();
        return (status > 0) ? NetworkState::NETWORK_CONNECTED : NetworkState::NETWORK_DISCONNECTED;
    }

    static inline std::string MAKE_FRIEND_CODE(uint64_t friendCode)
    {
        const auto first  = (int)((friendCode / 100000000) % 10000);
        const auto second = (int)((friendCode / 10000) % 10000);
        const auto third  = (int)((friendCode % 10000));

        return std::format("{:04}-{:04}-{:04}", first, second, third);
    }

    bool System::getFriendInfo(FriendInfo& info) const
    {
        FriendKey key {};
        uint64_t code = 0;

        if (!ResultCode(FRD_GetMyFriendKey(&key)))
            return false;

        if (!ResultCode(FRD_PrincipalIdToFriendCode(key.principalId, &code)))
            return false;

        bool valid = true;
        if (!ResultCode(FRD_IsValidFriendCode(code, &valid)) || !valid)
            return false;

        info.friendcode = MAKE_FRIEND_CODE(code);

        MiiScreenName name {};
        if (!ResultCode(FRD_GetMyScreenName(&name)))
            return false;

        info.username = utf8::utf16to8(reinterpret_cast<const char16_t*>(name));
        return true;
    }

    bool System::getInfo(ProductInfo& info) const
    {
        if (!ResultCode(CFGU_GetSystemModel(&info.model)))
            return false;

        OS_VersionBin nver {};
        OS_VersionBin cver {};

        if (!ResultCode(osGetSystemVersionData(&nver, &cver)))
            return false;

        info.version =
            std::format("{}.{}.{}-{}{}", cver.mainver, cver.minor, cver.build, nver.mainver, nver.region);

        if (!ResultCode(CFGU_SecureInfoGetRegion(&info.region)))
            return false;

        return true;
    }

    std::vector<std::string> System::getPreferredLocales() const
    {
        std::vector<std::string> locales {};

        CFG_Language language = (CFG_Language)-1;
        CFGU_GetSystemLanguage((uint8_t*)&language);

        std::string_view locale {};

        if (!System::getConstant(language, locale))
            locale = "Unknown";

        locales.push_back(std::string(locale));

        return locales;
    }

    static Handle openPlayCoinsFile(uint8_t mode = FS_OPEN_READ)
    {
        Handle playCoinsFile;
        const uint32_t path[3] = { MEDIATYPE_NAND, 0xF000000B, 0x00048000 };

        const FS_Path archivePath { PATH_BINARY, 0xC, path };
        const auto filePath  = fsMakePath(PATH_UTF16, u"/gamecoin.dat");
        const auto archiveId = ARCHIVE_SHARED_EXTDATA;

        if (!ResultCode(FSUSER_OpenFileDirectly(&playCoinsFile, archiveId, archivePath, filePath, mode, 0)))
            throw love::Exception("Failed to open gamecoin.dat.");

        return playCoinsFile;
    }

    void System::setPlayCoins(int amount)
    {
        if (amount < 0 || amount > 300)
            throw love::Exception("Cannot set Play Coin count to %d! Must be within [0, 300].", amount);

        Handle playCoinsFile    = openPlayCoinsFile(FS_OPEN_WRITE);
        const uint8_t buffer[2] = { (uint8_t)amount, (uint8_t)(amount >> 8) };

        if (!ResultCode(FSFILE_Write(playCoinsFile, nullptr, 4, buffer, 2, 0)))
        {
            FSFILE_Close(playCoinsFile);
            throw love::Exception("Failed to write to gamecoin.dat.");
        }

        FSFILE_Close(playCoinsFile);
    }

    int System::getPlayCoins() const
    {
        Handle playCoinsFile = openPlayCoinsFile();
        uint8_t buffer[2]    = { 0 };

        if (!ResultCode(FSFILE_Read(playCoinsFile, nullptr, 4, buffer, 2)))
        {
            FSFILE_Close(playCoinsFile);
            throw love::Exception("Failed to read gamecoin.dat.");
        }

        FSFILE_Close(playCoinsFile);

        return ((int)buffer[1] << 8) | buffer[0];
    }

    int System::getMemorySize() const
    {
        return osGetMemRegionSize(MEMREGION_ALL) / (1024 * 1024);
    }
} // namespace love
