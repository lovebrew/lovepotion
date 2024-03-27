#include "common/result.hpp"

#include "modules/system/System.hpp"

namespace love
{
    int System::getProcessorCount() const
    {
        uint8_t model = 0;
        CFGU_GetSystemModel(&model);

        int count = 2;

        switch (model)
        {
            default:
                break;
            case CFG_MODEL_N3DS:
            case CFG_MODEL_N3DSXL:
            case CFG_MODEL_N2DSXL:
            {
                count = 4;
                break;
            }
        }

        return count;
    }

    System::PowerState System::getPowerInfo(int& seconds, int& percent) const
    {
        uint8_t batteryState = 0;
        PowerState state     = PowerState::POWER_UNKNOWN;

        uint8_t percentRaw = 0;
        MCUHWC_GetBatteryLevel(&percentRaw);

        percent = (percentRaw / 0xFF) * 100;
        PTMU_GetBatteryChargeState(&batteryState);

        state = (batteryState) ? PowerState::POWER_CHARGING : PowerState::POWER_BATTERY;

        if (percent == 100 && !batteryState)
            state = PowerState::POWER_CHARGED;

        seconds = 0;

        return state;
    }

    System::NetworkState System::getNetworkInfo(uint8_t& signal) const
    {
        uint32_t status = 0;
        ACU_GetWifiStatus(&status);

        NetworkState state = NetworkState::NETWORK_UNKNOWN;

        signal = osGetWifiStrength();
        state = (status > 0) ? NetworkState::NETWORK_CONNECTED : NetworkState::NETWORK_DISCONNECTED;

        return state;
    }

    static inline std::string MAKE_FRIEND_CODE(uint64_t friendCode)
    {
        std::string result(0x0F, '\0');

        const auto first  = (int)((friendCode / 100000000) % 10000);
        const auto second = (int)((friendCode / 10000) % 10000);
        const auto third  = (int)((friendCode % 10000));

        snprintf(result.data(), result.size(), "%04i-%04i-%04i", first, second, third);

        return result;
    }

    System::FriendInfo System::getFriendInfo() const
    {
        FriendInfo info {};

        char username[0x1C] = { 0 };

        FriendKey key = {};
        uint64_t code = 0;

        if (R_SUCCEEDED(FRD_GetMyFriendKey(&key)))
        {
            if (R_SUCCEEDED(FRD_PrincipalIdToFriendCode(key.principalId, &code)))
                info.friendCode = MAKE_FRIEND_CODE(code);
        }

        if (R_SUCCEEDED(FRD_GetMyScreenName(username, 0x1C)))
            info.username = username;

        return info;
    }

    System::SystemInfo System::getOSInfo() const
    {
        CFG_SystemModel model = (CFG_SystemModel)-1;
        CFGU_GetSystemModel((uint8_t*)&model);

        std::string_view result {};
        if (!System::getConstant(model, result))
            result = "Unknown";

        char version[0x100] { 0 };
        if (R_FAILED(osGetSystemVersionDataString(NULL, NULL, version, 0x100)))
            return { std::string(result), "Unknown" };

        return { std::string(result), version };
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

    static Handle openPlayCoinsFile()
    {
        Handle playCoinsFile;
        const uint32_t path[3] = { MEDIATYPE_NAND, 0xF000000B, 0x00048000 };

        const FS_Path archivePath = { PATH_BINARY, 0xC, path };
        const FS_Path filePath    = fsMakePath(PATH_UTF16, u"/gamecoin.dat");

        Result res = FSUSER_OpenFileDirectly(&playCoinsFile, ARCHIVE_SHARED_EXTDATA, archivePath,
                                             filePath, FS_OPEN_READ | FS_OPEN_WRITE, 0);

        if (R_FAILED(res))
            throw love::Exception("Failed to open gamecoin.dat.");

        return playCoinsFile;
    }

    void System::setPlayCoins(int amount)
    {
        if (amount < 0 || amount > 300)
            throw love::Exception("Cannot set Play Coin count to %d! Must be within [0, 300].",
                                  amount);

        Handle playCoinsFile = openPlayCoinsFile();

        const uint8_t buffer[2] = { (uint8_t)amount, (uint8_t)(amount >> 8) };

        Result result = FSFILE_Write(playCoinsFile, nullptr, 4, buffer, 2, 0);

        if (R_FAILED(result))
        {
            FSFILE_Close(playCoinsFile);
            throw love::Exception("Failed to write to gamecoin.dat.");
        }

        FSFILE_Close(playCoinsFile);
    }

    int System::getPlayCoins() const
    {
        Handle playCoinsFile = openPlayCoinsFile();

        uint8_t buffer[2] = { 0 };

        Result result = FSFILE_Read(playCoinsFile, nullptr, 4, buffer, 2);

        if (R_FAILED(result))
        {
            FSFILE_Close(playCoinsFile);
            throw love::Exception("Failed to read gamecoin.dat.");
        }

        FSFILE_Close(playCoinsFile);

        return ((int)buffer[1] << 8) | buffer[0];
    }
} // namespace love
