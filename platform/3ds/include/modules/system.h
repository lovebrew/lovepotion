#pragma once

#include "modules/system/system.h"

#define USERNAME_LENGTH 0x1C

namespace love::ctr
{
    class System : public love::System
    {
        public:
            int GetProcessorCount() override;

            PowerInfo GetPowerInfo() const override;

            const std::string & GetUsername() override;

            NetworkInfo GetNetworkInfo() const override;

            const std::string & GetLanguage() override;
    };
}