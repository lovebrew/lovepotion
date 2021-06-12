#pragma once

#include "modules/system/systemc.h"

#define USERNAME_LENGTH 0x21
#define TEGRA_CPU_COUNT 0x04

namespace love
{
    class System : public common::System
    {
      public:
        System();

        virtual ~System() {};

        using common::System::GetPowerInfo;

        using common::System::GetNetworkInfo;

        int GetProcessorCount() override;

        const std::string& GetUsername() override;

        PowerState GetPowerInfo(uint8_t& percent) const override;

        NetworkState GetNetworkInfo(uint8_t& signal) const override;

        const std::string& GetSystemTheme() override;

        const std::string& GetLanguage() override;

        const std::string& GetModel() override;

        const std::string& GetRegion() override;

        const std::string& GetVersion() override;

        const std::string& GetFriendCode() override;

        static constexpr uint8_t MAX_REGIONS = 6;
        static constexpr uint8_t MAX_THEMES  = 2;
        static constexpr uint8_t MAX_MODELS  = 7;

        static bool GetConstant(const char* in, ColorSetId& out);
        static bool GetConstant(ColorSetId in, const char*& out);

        static bool GetConstant(const char* in, SetLanguage& out);
        static bool GetConstant(SetLanguage in, const char*& out);
        static std::vector<const char*> GetConstants(SetLanguage);

        static bool GetConstant(const char* in, SetSysProductModel& out);
        static bool GetConstant(SetSysProductModel in, const char*& out);
        static std::vector<const char*> GetConstants(SetSysProductModel);

        static bool GetConstant(const char* in, SetRegion& out);
        static bool GetConstant(SetRegion in, const char*& out);
        static std::vector<const char*> GetConstants(SetRegion);

      private:
        AccountUid userID;

        const static StringMap<SetSysProductModel, MAX_MODELS> models;
        const static StringMap<SetLanguage, SetLanguage_Total> languages;
        const static StringMap<SetRegion, MAX_REGIONS> regions;
        const static StringMap<ColorSetId, MAX_THEMES> themes;
    };
} // namespace love
