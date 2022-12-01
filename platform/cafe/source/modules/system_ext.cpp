#include <modules/system_ext.hpp>
#include <utilities/result.hpp>

#include <coreinit/core.h>
#include <coreinit/mcp.h>

#include <nn/ac/ac_c.h>
#include <nn/act/client_cpp.h>

#include <utilities/log/logfile.h>

#include <stdio.h>

using namespace love;

System<Console::CAFE>::System()
{
    this->handles.mcp        = MCP_Open();
    this->handles.userConfig = UCOpen();

    nn::act::Initialize();

    this->accountSlot = nn::act::GetDefaultAccount();
}

System<Console::CAFE>::~System()
{
    if (this->handles.mcp != 0)
        MCP_Close(this->handles.mcp);

    if (this->handles.userConfig != 0)
        UCClose(this->handles.userConfig);

    nn::act::Finalize();
}

System<>::PowerState System<Console::CAFE>::GetPowerInfo(uint8_t& percent) const
{
    percent = 100;

    return PowerState::POWER_UNKNOWN;
}

System<>::NetworkState System<Console::CAFE>::GetNetworkInfo(uint8_t& signal) const
{
    int32_t status     = 0;
    NetworkState state = NetworkState::NETWORK_UNKNOWN;

    R_UNLESS(ACIsApplicationConnected(&status).value, state);

    signal = (status > 0) ? 100 : 0;
    state  = (status > 0) ? NetworkState::NETWORK_CONNECTED : NetworkState::NETWORK_DISCONNECTED;

    return state;
}

int System<Console::CAFE>::GetProcessorCount()
{
    return OSGetCoreCount();
}

std::string_view System<Console::CAFE>::GetSystemTheme()
{
    return "light";
}

std::string_view System<Console::CAFE>::GetPreferredLocales()
{
    if (!this->info.locale.empty())
        return this->info.locale;

    UCSysConfig config {};
    uint32_t data = 0xFFFFFFFF;

    config.dataType = UC_DATATYPE_UNSIGNED_INT;
    config.dataSize = 0x04;
    config.data     = &data;

    strncpy(config.name, "cafe.language", 0x40);

    R_UNLESS(UCReadSysConfig(this->handles.userConfig, 1, &config), std::string {});

    int32_t languageData = (*(uint32_t*)config.data);

    std::optional<const char*> language;
    if (language = System::languages.ReverseFind((USCLanguage)languageData))
        this->info.locale = *language;
    else
        this->info.locale = "Unknown";

    this->info.locale += "_";

    MCPSysProdSettings settings {};
    R_UNLESS(MCP_GetSysProdSettings(this->handles.mcp, &settings), std::string {});

    std::optional<const char*> region;
    if (region = System::countryCodes.ReverseFind(settings.product_area))
        this->info.locale += *region;
    else
        this->info.locale += "Unknown";

    return this->info.locale;
}

std::string_view System<Console::CAFE>::GetVersion()
{
    if (!this->info.version.empty())
        return this->info.version;

    MCP_SystemVersion systemVersion {};
    R_UNLESS(MCP_GetSystemVersion(this->handles.mcp, &systemVersion), std::string {});

    std::string version {};
    sprintf(version.data(), "%d.%d.%d", systemVersion.major, systemVersion.minor,
            systemVersion.patch);

    this->info.version = version;

    return version;
}

std::string_view System<Console::CAFE>::GetModel()
{
    if (!this->info.model.empty())
        return this->info.model;

    BSPHardwareVersion version;
    R_UNLESS(bspGetHardwareVersion(&version), std::string {});

    std::optional<const char*> model;
    if (!(model = System::systemModels.ReverseFind((BSPHardwareVersions)version)))
        model = "Unknown";

    this->info.model = *model;

    return this->info.model;
}

std::string_view System<Console::CAFE>::GetUsername()
{
    int16_t outName[nn::act::MiiNameSize] { 0 };
    nn::act::GetMiiNameEx(outName, this->accountSlot);

    std::copy_n(outName, nn::act::MiiNameSize, this->info.username.data());

    return this->info.username;
}

std::string_view System<Console::CAFE>::GetFriendInfo()
{
    char username[nn::act::AccountIdSize];

    if (!nn::act::IsNetworkAccount())
        return std::string {};

    auto success = nn::act::GetAccountId(username);

    if (!success.IsSuccess())
        return std::string {};

    this->info.friendCode = username;
    return this->info.friendCode;
}
