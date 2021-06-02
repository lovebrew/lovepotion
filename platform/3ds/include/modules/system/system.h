#pragma once

#include "modules/system/systemc.h"

#define USERNAME_LENGTH 0x1C
static std::string THEME_NAME = "light";

namespace love
{
    class System : public common::System
    {
      public:
        virtual ~System()
        {}

        int GetProcessorCount() override;

        const std::string& GetModel() override;

        const std::string& GetUsername() override;

        PowerState GetPowerInfo(uint8_t& percent) const override;

        NetworkState GetNetworkInfo(uint8_t& signal) const override;

        const std::string& GetSystemTheme() override;

        const std::string& GetLanguage() override;

        const std::string& GetVersion() override;

        const std::string& GetRegion() override;

        const std::string& GetFriendCode() override;

        int GetPlayCoins() const;

        void SetPlayCoins(int);

        /* constants */

        static constexpr uint8_t MAX_MODELS    = 6;
        static constexpr uint8_t MAX_LANGUAGES = 12;
        static constexpr uint8_t MAX_REGIONS   = 7;

        static bool GetConstant(const char* in, CFG_Language& out);
        static bool GetConstant(CFG_Language in, const char*& out);
        static std::vector<const char*> GetConstants(CFG_Language);

        static bool GetConstant(const char* in, CFG_SystemModel& out);
        static bool GetConstant(CFG_SystemModel in, const char*& out);
        static std::vector<const char*> GetConstants(CFG_SystemModel);

        static bool GetConstant(const char* in, CFG_Region& out);
        static bool GetConstant(CFG_Region in, const char*& out);
        static std::vector<const char*> GetConstants(CFG_Region);

      private:
        static Handle OpenPlayCoinsFile();

        const static StringMap<CFG_SystemModel, MAX_MODELS> models;
        const static StringMap<CFG_Language, MAX_LANGUAGES> languages;
        const static StringMap<CFG_Region, MAX_REGIONS> regions;
    };
} // namespace love
