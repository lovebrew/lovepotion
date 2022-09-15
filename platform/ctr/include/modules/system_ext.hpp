#pragma once

#include <3ds.h>

#include <modules/system/system.tcc>

namespace love
{
    template<>
    class System<Console::CTR> : public System<Console::ALL>
    {
      public:
        PowerState GetPowerInfo(uint8_t& percent) const;

        NetworkState GetNetworkInfo(uint8_t& signal) const;

        int GetProcessorCount();

        std::string_view GetUsername();

        std::string_view GetSystemTheme();

        std::string_view GetPreferredLocales();

        std::string_view GetVersion();

        std::string_view GetModel();

        std::string_view GetFriendInfo();

        int GetPlayCoins() const;

        void SetPlayCoins(int amount);

        static bool GetConstant(const char* in, CFG_Language& out);
        static bool GetConstant(CFG_Language in, const char*& out);
        static std::vector<const char*> GetConstants(CFG_Language);

        static bool GetConstant(const char* in, CFG_SystemModel& out);
        static bool GetConstant(CFG_SystemModel in, const char*& out);
        static std::vector<const char*> GetConstants(CFG_SystemModel);

        static bool GetConstant(const char* in, CFG_Region& out);
        static bool GetConstant(CFG_Region in, const char*& out);
        static std::vector<const char*> GetConstants(CFG_Region);
    };
} // namespace love
