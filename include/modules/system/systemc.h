#pragma once

#include "common/module.h"

using namespace std::literals::string_literals;

#define OS_NAME "Horizon"

#if defined(__3DS__)
    #define LANGUAGE_COUNT 12
#elif defined(__SWITCH__)
    #define LANGUAGE_COUNT 17
#endif

namespace love::common
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

        enum SystemTheme
        {
            THEME_LIGHT,
            THEME_DARK,
            THEME_MAX_ENUM
        };

        System();

        ModuleType GetModuleType() const
        {
            return M_SYSTEM;
        }

        const char* GetName() const override
        {
            return "love.system";
        }

        /* LÖVE2D Functions */

        std::string GetOS() const;

        virtual PowerState GetPowerInfo(uint8_t& percent) const = 0;

        virtual NetworkState GetNetworkInfo(uint8_t& signal) const = 0;

        /* pure virtual subclass stuff */

        virtual int GetProcessorCount() = 0;

        virtual const std::string& GetUsername() = 0;

        virtual const std::string& GetSystemTheme() = 0;

        virtual const std::string& GetPreferredLocales() = 0;

        virtual const std::string& GetVersion() = 0;

        virtual const std::string& GetRegion() = 0;

        virtual const std::string& GetModel() = 0;

        virtual const std::string& GetFriendCode() = 0;

        /* end pure virtual methods */

        static bool GetConstant(const char* in, PowerState& out);
        static bool GetConstant(PowerState in, const char*& out);

        static bool GetConstant(const char* in, NetworkState& out);
        static bool GetConstant(NetworkState in, const char*& out);

      protected:
        struct
        {
            int processors;
            std::string model;
            std::string region;
            std::string version;
            std::string username;
            std::string language;
            std::string friendCode;
            std::string colorTheme;
        } systemInfo;
    };
} // namespace love::common
