#include "modules/system/systemc.h"

using namespace love::common;

System::System() : systemInfo()
{ /* initialize sysInfo defaults */
}

std::string System::GetOS() const
{
    return OS_NAME;
}

System::PowerState System::GetPowerInfo(int& percent) const
{
    PowerState state = POWER_UNKNOWN;

    PowerInfo info = this->GetPowerInfo();

    this->GetConstant(info.state.c_str(), state);
    percent = info.percentage;

    return state;
}

System::NetworkState System::GetNetworkInfo(int& signal) const
{
    NetworkState state = NETWORK_UNKNOWN;

    NetworkInfo info = this->GetNetworkInfo();

    this->GetConstant(info.status.c_str(), state);
    signal = info.signal;

    return state;
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

const StringMap<System::PowerState, System::POWER_MAX_ENUM> System::powerStates(powerEntries);

constexpr StringMap<System::NetworkState, System::NETWORK_MAX_ENUM>::Entry networkEntries[] =
{
    { "unknown",      System::NetworkState::NETWORK_UNKNOWN      },
    { "connected",    System::NetworkState::NETWORK_CONNECTED    },
    { "disconnected", System::NetworkState::NETWORK_DISCONNECTED }
};

const StringMap<System::NetworkState, System::NETWORK_MAX_ENUM> System::networkStates(networkEntries);
// clang-format on
