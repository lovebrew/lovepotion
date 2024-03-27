#pragma once

#include "common/module.hpp"
#include "utility/map.hpp"

namespace love
{
    template<class T>
    class SystemBase : public Module
    {
      public:
        enum PowerState
        {
            POWER_UNKNOWN,
            POWER_BATTERY,
            POWER_NO_BATTERY,
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

        SystemBase() : Module(M_SYSTEM, "love.system")
        {}

        virtual ~SystemBase()
        {}

        static const char* getOS()
        {
            return __OS__;
        }

        void setClipboardText(const std::string& text)
        {
            this->clipboard = text;
        }

        const std::string& getClipboardText() const
        {
            return this->clipboard;
        }

        struct FriendInfo
        {
            std::string username;
            std::string friendCode;
        };

        struct SystemInfo
        {
            std::string version;
            std::string model;
        };

        // clang-format off
        STRINGMAP_DECLARE(powerStates, PowerState,
            { "unknown",   POWER_UNKNOWN    },
            { "battery",   POWER_BATTERY    },
            { "nobattery", POWER_NO_BATTERY },
            { "charging",  POWER_CHARGING   },
            { "charged",   POWER_CHARGED    }
        );

        STRINGMAP_DECLARE(networkStates, NetworkState,
            { "unknown",      NETWORK_UNKNOWN      },
            { "disconnected", NETWORK_DISCONNECTED },
            { "connected",    NETWORK_CONNECTED    }
        );
        // clang-format on

      private:
        std::string clipboard;
    };
} // namespace love
