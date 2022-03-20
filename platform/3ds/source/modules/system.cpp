#include "modules/system/system.h"
#include <3ds.h>

#include "common/bidirectionalmap.h"
#include "common/results.h"

using namespace love;

int System::GetProcessorCount()
{
    if (this->systemInfo.processors != 0)
        return this->systemInfo.processors;

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

    this->systemInfo.processors = processorCount;

    return this->systemInfo.processors;
}

const std::string& System::GetModel()
{
    if (!this->systemInfo.model.empty())
        return this->systemInfo.model;

    uint8_t model = 0;

    R_UNLESS(CFGU_GetSystemModel(&model), LOVE_STRING_EMPTY);

    const char* name = nullptr;
    if (!System::GetConstant(static_cast<CFG_SystemModel>(model), name))
        name = "Unknown";

    this->systemInfo.model = name;

    return this->systemInfo.model;
}

const std::string& System::GetUsername()
{
    if (!this->systemInfo.username.empty())
        return this->systemInfo.username;

    char username[USERNAME_LENGTH] = { 0 };

    R_UNLESS(FRD_GetMyScreenName(username, USERNAME_LENGTH), LOVE_STRING_EMPTY);

    this->systemInfo.username = username;

    return this->systemInfo.username;
}

const std::string& System::GetRegion()
{
    if (!this->systemInfo.region.empty())
        return this->systemInfo.region;

    uint8_t region = 0;

    R_UNLESS(CFGU_SecureInfoGetRegion(&region), LOVE_STRING_EMPTY);

    const char* name = nullptr;
    if (!System::GetConstant(static_cast<CFG_Region>(region), name))
        name = "Unknown";

    this->systemInfo.region = name;

    return this->systemInfo.region;
}

System::PowerState System::GetPowerInfo(uint8_t& percent) const
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

System::NetworkState System::GetNetworkInfo(uint8_t& signal) const
{
    uint32_t status = 0;
    ACU_GetWifiStatus(&status);

    NetworkState state = NetworkState::NETWORK_UNKNOWN;

    signal = osGetWifiStrength();
    state  = (status > 0) ? NetworkState::NETWORK_CONNECTED : NetworkState::NETWORK_DISCONNECTED;

    return state;
}

const std::string& System::GetPreferredLocales()
{
    if (!this->systemInfo.language.empty())
        return this->systemInfo.language;

    uint8_t language;

    R_UNLESS(CFGU_GetSystemLanguage(&language), LOVE_STRING_EMPTY);

    const char* name = nullptr;
    if (!System::GetConstant(static_cast<CFG_Language>(language), name))
        name = "Unknown";

    this->systemInfo.language = name;

    return this->systemInfo.language;
}

const std::string& System::GetVersion()
{
    if (!this->systemInfo.version.empty())
        return this->systemInfo.version;

    char out[256] = { 0 };

    R_UNLESS(osGetSystemVersionDataString(NULL, NULL, out, 256), LOVE_STRING_EMPTY);

    this->systemInfo.version = out;
    return this->systemInfo.version;
}

/* Friend Code stuff */

static inline std::string MAKE_FRIEND_CODE(uint64_t friendCode)
{
    std::string out = "####-####-####";

    sprintf(out.data(), "%04i-%04i-%04i", (int)(friendCode / 100000000) % 10000,
            (int)(friendCode / 10000) % 10000, (int)(friendCode % 10000));

    return out;
}

const std::string& System::GetFriendCode()
{
    if (!this->systemInfo.friendCode.empty())
        return this->systemInfo.friendCode;

    FriendKey key;
    uint64_t friendCode;

    /* Get the Friend Key for the user */
    R_UNLESS(FRD_GetMyFriendKey(&key), LOVE_STRING_EMPTY);

    /* Convert the principalId to friendCode */
    R_UNLESS(FRD_PrincipalIdToFriendCode(key.principalId, &friendCode), LOVE_STRING_EMPTY);

    this->systemInfo.friendCode = MAKE_FRIEND_CODE(friendCode);

    return this->systemInfo.friendCode;
}

const std::string& System::GetSystemTheme()
{
    return THEME_NAME;
}

Handle System::OpenPlayCoinsFile()
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

int System::GetPlayCoins() const
{
    Handle playCoinsFile = System::OpenPlayCoinsFile();

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

void System::SetPlayCoins(int amount)
{
    if (amount < 0 || amount > 300)
        throw love::Exception("Cannot set Play Coin count to %d! Must be within [0, 300].", amount);

    Handle playCoinsFile = System::OpenPlayCoinsFile();

    const uint8_t buffer[2] = { (uint8_t)amount, (uint8_t)(amount >> 8) };

    Result res = FSFILE_Write(playCoinsFile, nullptr, 4, buffer, 2, 0);

    if (R_FAILED(res))
    {
        FSFILE_Close(playCoinsFile);
        throw love::Exception("Failed to write to gamecoin.dat!");
    }

    FSFILE_Close(playCoinsFile);
}

// clang-format off
constexpr auto languages = BidirectionalMap<>::Create(
    "jp",    CFG_LANGUAGE_JP,
    "en",    CFG_LANGUAGE_EN,
    "fr",    CFG_LANGUAGE_FR,
    "de",    CFG_LANGUAGE_DE,
    "it",    CFG_LANGUAGE_IT,
    "es",    CFG_LANGUAGE_ES,
    "zh_CN", CFG_LANGUAGE_ZH,
    "ko",    CFG_LANGUAGE_KO,
    "nl",    CFG_LANGUAGE_NL,
    "pt",    CFG_LANGUAGE_PT,
    "ru",    CFG_LANGUAGE_RU,
    "zh_TW", CFG_LANGUAGE_TW
);

constexpr auto models = BidirectionalMap<>::Create(
    "3DS",       CFG_MODEL_3DS,
    "3DSXL",     CFG_MODEL_3DSXL,
    "New 3DS",   CFG_MODEL_N3DS,
    "2DS",       CFG_MODEL_2DS,
    "New 3DSXL", CFG_MODEL_N3DSXL,
    "New 2DSXL", CFG_MODEL_N2DSXL
);

constexpr auto regions = BidirectionalMap<>::Create(
    "Japan",         CFG_REGION_JPN,
    "United States", CFG_REGION_USA,
    "Europe",        CFG_REGION_EUR,
    "Australia",     CFG_REGION_AUS,
    "China",         CFG_REGION_CHN,
    "Korea",         CFG_REGION_KOR,
    "Taiwan",        CFG_REGION_TWN
);
// clang-format on

/* LANGUAGE CONSTANTS */

bool System::GetConstant(const char* in, CFG_Language& out)
{
    return languages.Find(in, out);
}

bool System::GetConstant(CFG_Language in, const char*& out)
{
    return languages.ReverseFind(in, out);
}

std::vector<const char*> System::GetConstants(CFG_Language)
{
    return languages.GetNames();
}

/* MODEL CONSTANTS */

bool System::GetConstant(const char* in, CFG_SystemModel& out)
{
    return models.Find(in, out);
}

bool System::GetConstant(CFG_SystemModel in, const char*& out)
{
    return models.ReverseFind(in, out);
}

std::vector<const char*> System::GetConstants(CFG_SystemModel)
{
    return models.GetNames();
}

/* REGION CONSTANTS */

bool System::GetConstant(const char* in, CFG_Region& out)
{
    return regions.Find(in, out);
}

bool System::GetConstant(CFG_Region in, const char*& out)
{
    return regions.ReverseFind(in, out);
}

std::vector<const char*> System::GetConstants(CFG_Region)
{
    return regions.GetNames();
}
