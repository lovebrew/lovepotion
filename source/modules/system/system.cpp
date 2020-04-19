#include "common/runtime.h"
#include "modules/system/system.h"

using namespace love;

std::string System::GetOS() const
{
    return OS_NAME;
}

int System::GetProcessorCount() const
{
    return Horizon::GetCPUCount();
}

System::PowerState System::GetPowerInfo(int & percent) const
{
    PowerState state = POWER_UNKNOWN;

    Horizon::PowerInfo info = Horizon::GetPowerInfo();

    this->GetConstant(info.state.c_str(), state);
    percent = info.percentage;

    return state;
}

System::NetworkState System::GetNetworkInfo(int & signal) const
{
    NetworkState state = NETWORK_UNKNOWN;

    Horizon::NetworkInfo info = Horizon::GetNetworkInfo();

    this->GetConstant(info.status.c_str(), state);
    signal = info.signal;

    return state;
}

std::string System::GetUsername() const
{
    return Horizon::GetUsername();
}

std::string System::GetLanguage() const
{
    return Horizon::GetSystemLanguage();
}

bool System::GetConstant(const char * in, System::PowerState & out)
{
	return powerStates.Find(in, out);
}

bool System::GetConstant(System::PowerState in, const char *& out)
{
	return powerStates.Find(in, out);
}

bool System::GetConstant(const char * in, System::NetworkState & out)
{
	return networkStates.Find(in, out);
}

bool System::GetConstant(System::NetworkState in, const char *& out)
{
	return networkStates.Find(in, out);
}

StringMap<System::PowerState, System::POWER_MAX_ENUM>::Entry System::powerEntries[] =
{
    { "unknown",  System::POWER_UNKNOWN   },
    { "battery",  System::POWER_BATTERY   },
    { "charging", System::POWER_CHARGING  },
    { "charged",  System::POWER_CHARGED   }
};

StringMap<System::PowerState, System::POWER_MAX_ENUM> System::powerStates(System::powerEntries, sizeof(System::powerEntries));

StringMap<System::NetworkState, System::NETWORK_MAX_ENUM>::Entry System::networkEntries[] =
{
    { "unknown",      System::NETWORK_UNKNOWN      },
    { "disconnected", System::NETWORK_DISCONNECTED },
    { "connected",    System::NETWORK_CONNECTED    },
};

StringMap<System::NetworkState, System::NETWORK_MAX_ENUM> System::networkStates(System::networkEntries, sizeof(System::networkEntries));
