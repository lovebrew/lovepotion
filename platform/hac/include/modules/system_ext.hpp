#pragma once

#include <modules/system/system.tcc>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

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

        // clang-format off
        static constexpr BidirectionalMap languages = {
            "jp",      SetLanguage_JA,
            "en_US",   SetLanguage_ENUS,
            "fr",      SetLanguage_FR,
            "de",      SetLanguage_DE,
            "it",      SetLanguage_IT,
            "es",      SetLanguage_ES,
            "zh_CN",   SetLanguage_ZHCN,
            "ko",      SetLanguage_KO,
            "nl",      SetLanguage_NL,
            "pt",      SetLanguage_PT,
            "ru",      SetLanguage_RU,
            "zh_TW",   SetLanguage_ZHTW,
            "en_GB",   SetLanguage_ENGB,
            "fr_CA",   SetLanguage_FRCA,
            "es_419",  SetLanguage_ES419,
            "zh_HANS", SetLanguage_ZHHANS,
            "zh_HANT", SetLanguage_ZHHANT,
            "pt_BR",   SetLanguage_PTBR
        };

        static constexpr BidirectionalMap models = {
            "Invalid",           SetSysProductModel_Invalid,
            "Erista",            SetSysProductModel_Nx,
            "Erista Simulation", SetSysProductModel_Copper,
            "Mariko",            SetSysProductModel_Iowa,
            "Mariko Lite",       SetSysProductModel_Hoag,
            "Mariko Simulation", SetSysProductModel_Calcio,
            "Mariko Pro",        SetSysProductModel_Aula
        };

        static constexpr BidirectionalMap themes = {
            "dark",  ColorSetId_Dark,
            "light", ColorSetId_Light
        };
        // clang-format on

      private:
        AccountUid account;
    };
} // namespace love
