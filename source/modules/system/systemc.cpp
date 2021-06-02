#include "modules/system/systemc.h"

using namespace love::common;

System::System() : systemInfo()
{ /* initialize sysInfo defaults */
}

std::string System::GetOS() const
{
    return OS_NAME;
}

bool System::GetConstant(const char* in, System::PowerState& out)
{
    return powerStates.Find(in, out);
}

bool System::GetConstant(System::PowerState in, const char*& out)
{
    return powerStates.Find(in, out);
}

bool System::GetConstant(const char* in, System::NetworkState& out)
{
    return networkStates.Find(in, out);
}

bool System::GetConstant(System::NetworkState in, const char*& out)
{
    return networkStates.Find(in, out);
}

// clang-format off
constexpr StringMap<System::PowerState, System::POWER_MAX_ENUM>::Entry powerEntries[] =
{
    { "unknown",  System::PowerState::POWER_UNKNOWN  },
    { "battery",  System::PowerState::POWER_BATTERY  },
    { "charged",  System::PowerState::POWER_CHARGED  },
    { "charging", System::PowerState::POWER_CHARGING }
};

constinit const StringMap<System::PowerState, System::POWER_MAX_ENUM> System::powerStates(powerEntries);

constexpr StringMap<System::NetworkState, System::NETWORK_MAX_ENUM>::Entry networkEntries[] =
{
    { "unknown",      System::NetworkState::NETWORK_UNKNOWN      },
    { "connected",    System::NetworkState::NETWORK_CONNECTED    },
    { "disconnected", System::NetworkState::NETWORK_DISCONNECTED }
};

constinit const StringMap<System::NetworkState, System::NETWORK_MAX_ENUM> System::networkStates(networkEntries);
// clang-format on
