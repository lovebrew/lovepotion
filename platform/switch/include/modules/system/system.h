#pragma once

#include "modules/system/systemc.h"

#define USERNAME_LENGTH 0x21
#define TEGRA_CPU_COUNT 0x04

namespace love
{
    class System : public common::System
    {
        public:
            virtual ~System() {};

            using common::System::GetPowerInfo;

            using common::System::GetNetworkInfo;

            int GetProcessorCount() override;

            PowerInfo GetPowerInfo() const override;

            const std::string & GetUsername() override;

            NetworkInfo GetNetworkInfo() const override;

            const std::string & GetLanguage() override;

            const std::string & GetVersion() override;
    };
}