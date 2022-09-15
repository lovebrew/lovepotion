#pragma once

#include <modules/system/system.tcc>

#include <switch.h>

namespace love
{
    template<>
    class System<Console::HAC> : public System<Console::ALL>
    {
      public:
        System();

        PowerState GetPowerInfo(uint8_t& percent) const;

        NetworkState GetNetworkInfo(uint8_t& signal) const;

        int GetProcessorCount();

        std::string_view GetUsername();

        std::string_view GetSystemTheme();

        std::string_view GetPreferredLocales();

        std::string_view GetVersion();

        std::string_view GetModel();

        std::string_view GetFriendInfo();

        static bool GetConstant(const char* in, ColorSetId& out);
        static bool GetConstant(ColorSetId in, const char*& out);

        static bool GetConstant(const char* in, SetLanguage& out);
        static bool GetConstant(SetLanguage in, const char*& out);
        static std::vector<const char*> GetConstants(SetLanguage);

        static bool GetConstant(const char* in, SetSysProductModel& out);
        static bool GetConstant(SetSysProductModel in, const char*& out);
        static std::vector<const char*> GetConstants(SetSysProductModel);

      private:
        AccountUid account;
    };
} // namespace love
