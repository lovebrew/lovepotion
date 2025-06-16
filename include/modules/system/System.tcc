#pragma once

#include "common/Map.hpp"
#include "common/Module.hpp"

namespace love
{
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

        struct ProductInfo
        {
            std::string version;
            std::string model;
            std::string region;
        };

        // clang-format off
        STRINGMAP_DECLARE(PowerStates, PowerState,
            { "unknown",   POWER_UNKNOWN    },
            { "battery",   POWER_BATTERY    },
            { "nobattery", POWER_NO_BATTERY },
            { "charging",  POWER_CHARGING   },
            { "charged",   POWER_CHARGED    }
        );

        STRINGMAP_DECLARE(NetworkStates, NetworkState,
            { "unknown",      NETWORK_UNKNOWN      },
            { "disconnected", NETWORK_DISCONNECTED },
            { "connected",    NETWORK_CONNECTED    }
        );
        // clang-format on

      private:
        std::string clipboard;
    };
} // namespace love
