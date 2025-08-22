#include <3ds.h>

#include <utf8.h>
#include <utilities/result.hpp>

#include <modules/system_ext.hpp>

using namespace love;

System<>::PowerState System<Console::CTR>::GetPowerInfo(uint8_t& percent) const
{
    uint8_t batteryState = 0;
    PowerState state     = PowerState::POWER_UNKNOWN;

    MCUHWC_GetBatteryLevel(&percent);
    PTMU_GetBatteryChargeState(&batteryState);

    state = (batteryState) ? PowerState::POWER_CHARGING : PowerState::POWER_BATTERY;

    if (percent == 100 && !batteryState)
        state = PowerState::POWER_CHARGED;

    return state;
}

System<>::NetworkState System<Console::CTR>::GetNetworkInfo(uint8_t& signal) const
{
    uint32_t status = 0;
    ACU_GetWifiStatus(&status);

    NetworkState state = NetworkState::NETWORK_UNKNOWN;

    signal = osGetWifiStrength();
    state  = (status > 0) ? NetworkState::NETWORK_CONNECTED : NetworkState::NETWORK_DISCONNECTED;

    return state;
}

int System<Console::CTR>::GetProcessorCount()
{
    if (this->info.processors != 0)
        return this->info.processors;

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

    this->info.processors = count;

    return count;
}

std::string_view System<Console::CTR>::GetSystemTheme()
{
    return "light";
}

std::string_view System<Console::CTR>::GetPreferredLocales()
{
    if (!this->info.locale.empty())
        return this->info.locale;

    uint8_t language = 0;
    R_UNLESS(CFGU_GetSystemLanguage(&language), std::string {});

    if (auto lang = System::languages.ReverseFind((CFG_Language)language))
        this->info.locale = *lang;
    else
        this->info.locale = "Unknown";

    this->info.locale += "_";

    uint8_t region = 0;
    R_UNLESS(CFGU_SecureInfoGetRegion(&region), std::string {});

    if (auto r = System::countryCodes.ReverseFind((CFG_Region)region))
        this->info.locale += *r;
    else
        this->info.locale += "Unknown";

    return this->info.locale;
}

std::string_view System<Console::CTR>::GetVersion()
{
    if (!this->info.version.empty())
        return this->info.version;

    char version[256] { 0 };

    R_UNLESS(osGetSystemVersionDataString(NULL, NULL, version, 256), std::string {});

    this->info.version = version;

    return version;
}

std::string_view System<Console::CTR>::GetModel()
{
    if (!this->info.model.empty())
        return this->info.model;

    uint8_t model = 0;
    R_UNLESS(CFGU_GetSystemModel(&model), std::string {});

    if (auto modelName = System::models.ReverseFind((CFG_SystemModel)model))
        this->info.model = *modelName;
    else
        this->info.model = "Unknown";

    return this->info.model;
}

std::string_view System<Console::CTR>::GetUsername()
{
    if (!this->info.username.empty())
        return this->info.username;

    MiiScreenName name {};
    R_UNLESS(FRD_GetMyScreenName(&name), std::string {});

    this->info.username = utf8::utf16to8(reinterpret_cast<const char16_t*>(name));
    return this->info.username;
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

std::string_view System<Console::CTR>::GetFriendInfo()
{
    if (!this->info.friendCode.empty())
        return this->info.friendCode;

    FriendKey friendKey {};
    uint64_t friendCode = 0;

    /* Get the Friend Key for the user */
    R_UNLESS(FRD_GetMyFriendKey(&friendKey), std::string {});

    /* Convert the principalId to friendCode */
    R_UNLESS(FRD_PrincipalIdToFriendCode(friendKey.principalId, &friendCode), std::string {});

    this->info.friendCode = MAKE_FRIEND_CODE(friendCode);

    return this->info.friendCode;
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
        throw love::Exception("Failed to open gamecoin.dat!");

    return playCoinsFile;
}

void System<Console::CTR>::SetPlayCoins(int amount)
{
    if (amount < 0 || amount > 300)
        throw love::Exception("Cannot set Play Coin count to %d! Must be within [0, 300].", amount);

    Handle playCoinsFile = openPlayCoinsFile();

    const uint8_t buffer[2] = { (uint8_t)amount, (uint8_t)(amount >> 8) };

    Result res = FSFILE_Write(playCoinsFile, nullptr, 4, buffer, 2, 0);

    if (R_FAILED(res))
    {
        FSFILE_Close(playCoinsFile);
        throw love::Exception("Failed to write to gamecoin.dat!");
    }

    FSFILE_Close(playCoinsFile);
}

int System<Console::CTR>::GetPlayCoins() const
{
    Handle playCoinsFile = openPlayCoinsFile();

    uint8_t buffer[2] = { 0 };

    Result res = FSFILE_Read(playCoinsFile, nullptr, 4, buffer, 2);

    if (R_FAILED(res))
    {
        FSFILE_Close(playCoinsFile);
        throw love::Exception("Failed to read gamecoin.dat!");
    }

    FSFILE_Close(playCoinsFile);

    return ((int)buffer[1] << 8) | buffer[0];
}
