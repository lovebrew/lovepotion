#include <modules/system_ext.hpp>
#include <utilities/result.hpp>

#include <coreinit/core.h>
#include <coreinit/mcp.h>

#include <nn/ac/ac_c.h>
#include <nn/act/client_cpp.h>

#include <stdio.h>

using namespace love;

System<Console::CAFE>::System()
{
    this->handle = MCP_Open();
    ACInitialize();

    this->slot = nn::act::GetDefaultAccount();
}

System<Console::CAFE>::~System()
{
    if (this->handle != 0)
        MCP_Close(this->handle);
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
    return std::string {};
}

std::string_view System<Console::CAFE>::GetVersion()
{
    if (!this->info.version.empty())
        return this->info.version;

    MCP_SystemVersion systemVersion {};
    R_UNLESS(MCP_GetSystemVersion(this->handle, &systemVersion), std::string {});

    std::string version {};
    sprintf(version.data(), "%d.%d.%d", systemVersion.major, systemVersion.minor,
            systemVersion.patch);

    this->info.version = version;

    return version;
}

std::string_view System<Console::CAFE>::GetModel()
{
    return std::string {};
}

std::string_view System<Console::CAFE>::GetUsername()
{
    int16_t outName[nn::act::MiiNameSize] { 0 };
    nn::act::GetMiiNameEx(outName, this->slot);

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

// clang-format off
constexpr auto modelTypes = BidirectionalMap<>::Create(
    "basic",  System<Console::CAFE>::SYSTEM_MODEL_BASIC,
    "deluxe", System<Console::CAFE>::SYSTEM_MODEL_DELUXE
);
// clang-format on

bool System<Console::CAFE>::GetConstant(const char* in, SystemModel& out)
{
    return modelTypes.Find(in, out);
}

bool System<Console::CAFE>::GetConstant(SystemModel in, const char*& out)
{
    return modelTypes.ReverseFind(in, out);
}
