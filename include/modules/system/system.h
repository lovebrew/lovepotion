#pragma once

#include "common/stringmap.h"
#include "common/backend/horizon.h"

#define OS_NAME "Horizon"

namespace love
{
    class System : public Module
    {
        public:
            enum PowerState
            {
                POWER_UNKNOWN,
                POWER_BATTERY,
                POWER_CHARGING,
                POWER_CHARGED,
                POWER_MAX_ENUM
            };

            enum NetworkState
            {
                NETWORK_UNKNOWN,
                NETWORK_DISCONNECTED,
                NETWORK_CONNECTED,
                NETWORK_MAX_ENUM
            };

            ModuleType GetModuleType() const { return M_SYSTEM; }

            const char * GetName() const override { return "love.system"; }

            /* LÖVE2D Functions */

            std::string GetOS() const;

            int GetProcessorCount() const;

            PowerState GetPowerInfo(int & percent) const;

            std::string GetUsername() const;

            std::string GetLanguage() const;

            NetworkState GetNetworkInfo(int & signal) const;

            static bool GetConstant(const char * in, PowerState & out);
            static bool GetConstant(PowerState in, const char *& out);

            static bool GetConstant(const char * in, NetworkState & out);
            static bool GetConstant(NetworkState in, const char *& out);

        private:
            static StringMap<PowerState, POWER_MAX_ENUM>::Entry powerEntries[];
            static StringMap<PowerState, POWER_MAX_ENUM> powerStates;

            static StringMap<NetworkState, NETWORK_MAX_ENUM>::Entry networkEntries[];
            static StringMap<NetworkState, NETWORK_MAX_ENUM> networkStates;
    };
}
