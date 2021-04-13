#pragma once

#include "modules/system/systemc.h"

#define USERNAME_LENGTH 0x21
#define TEGRA_CPU_COUNT 0x04

namespace love
{
    class System : public common::System
    {
      public:
        enum ProductModel
        {
            MODEL_INVALID,
            MODEL_NX,
            MODEL_COPPER,
            MODEL_IOWA,
            MODEL_HOAG,
            MODEL_CALCIO,
            MODEL_AULA,
            MODEL_MAX_ENUM
        };

        virtual ~System() {};

        using common::System::GetPowerInfo;

        using common::System::GetNetworkInfo;

        int GetProcessorCount() override;

        PowerInfo GetPowerInfo() const override;

        const std::string& GetUsername() override;

        NetworkInfo GetNetworkInfo() const override;

        const std::string& GetLanguage() override;

        const std::string& GetModel() override;

        const std::string& GetRegion() override;

        const std::string& GetVersion() override;

        const std::string& GetFriendCode() override;

        static constexpr uint8_t MAX_REGIONS = 6;

        static bool GetConstant(const char* in, SetLanguage& out);
        static bool GetConstant(SetLanguage in, const char*& out);
        static std::vector<std::string> GetConstants(SetLanguage);

        static bool GetConstant(const char* in, ProductModel& out);
        static bool GetConstant(ProductModel in, const char*& out);
        static std::vector<std::string> GetConstants(ProductModel);

        static bool GetConstant(const char* in, SetRegion& out);
        static bool GetConstant(SetRegion in, const char*& out);
        static std::vector<std::string> GetConstants(SetRegion);

      private:
        static StringMap<ProductModel, MODEL_MAX_ENUM>::Entry modelEntries[];
        static StringMap<ProductModel, MODEL_MAX_ENUM> models;

        static StringMap<SetLanguage, SetLanguage_Total>::Entry languageEntries[];
        static StringMap<SetLanguage, SetLanguage_Total> languages;

        static StringMap<SetRegion, MAX_REGIONS>::Entry regionEntries[];
        static StringMap<SetRegion, MAX_REGIONS> regions;
    };
} // namespace love
