#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    template<Console::Platform T = Console::ALL>
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

        System() : info {}
        {}

        ModuleType GetModuleType() const override
        {
            return M_SYSTEM;
        }

        const char* GetName() const override
        {
            return "love.system";
        }

        std::string_view GetOS() const
        {
            return __OS__;
        }

        // clang-format off
        static constexpr BidirectionalMap powerStates = {
            "unknown",  System::PowerState::POWER_UNKNOWN,
            "battery",  System::PowerState::POWER_BATTERY,
            "charged",  System::PowerState::POWER_CHARGED,
            "charging", System::PowerState::POWER_CHARGING
        };

        static constexpr BidirectionalMap networkStates = {
            "unknown",      System::NetworkState::NETWORK_UNKNOWN,
            "connected",    System::NetworkState::NETWORK_CONNECTED,
            "disconnected", System::NetworkState::NETWORK_DISCONNECTED
        };
        // clang-format on

      protected:
        struct
        {
            size_t processors;
            std::string model;
            std::string locale;
            std::string version;
            std::string username;
            std::string friendCode;
            std::string colorTheme;
        } info;

      private:
    };
} // namespace love
