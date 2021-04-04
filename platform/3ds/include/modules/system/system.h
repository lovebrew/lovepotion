#pragma once

#include "modules/system/systemc.h"

#define USERNAME_LENGTH 0x1C

namespace love
{
    class System : public common::System
    {
        public:
            virtual ~System() {}

            using common::System::GetPowerInfo;

            using common::System::GetNetworkInfo;

            int GetProcessorCount() override;

            const std::string & GetModel() override;

            PowerInfo GetPowerInfo() const override;

            const std::string & GetUsername() override;

            NetworkInfo GetNetworkInfo() const override;

            const std::string & GetLanguage() override;

            const std::string & GetVersion() override;

            const std::string & GetRegion() override;

            const std::string & GetFriendCode() override;

            /* constants */

            static constexpr uint8_t MAX_MODELS = 6;
            static constexpr uint8_t MAX_LANGUAGES = 12;
            static constexpr uint8_t MAX_REGIONS = 7;

            static bool GetConstant(const char * in, CFG_Language & out);
            static bool GetConstant(CFG_Language in, const char *& out);
            static std::vector<std::string> GetConstants(CFG_Language);

            static bool GetConstant(const char * in, CFG_SystemModel & out);
            static bool GetConstant(CFG_SystemModel in, const char *& out);
            static std::vector<std::string> GetConstants(CFG_SystemModel);

            static bool GetConstant(const char * in, CFG_Region & out);
            static bool GetConstant(CFG_Region in, const char *& out);
            static std::vector<std::string> GetConstants(CFG_Region);

        private:
            static StringMap<CFG_SystemModel, MAX_MODELS>::Entry modelEntries[];
            static StringMap<CFG_SystemModel, MAX_MODELS> models;

            static StringMap<CFG_Language, MAX_LANGUAGES>::Entry languageEntries[];
            static StringMap<CFG_Language, MAX_LANGUAGES> languages;

            static StringMap<CFG_Region, MAX_REGIONS>::Entry regionEntries[];
            static StringMap<CFG_Region, MAX_REGIONS> regions;
    };
}
