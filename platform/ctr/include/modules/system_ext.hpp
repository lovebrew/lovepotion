#pragma once

#include <3ds.h>

#include <modules/system/system.tcc>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

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

        // clang-format off
        static constexpr BidirectionalMap languages = {
            "jp",    CFG_LANGUAGE_JP,
            "en",    CFG_LANGUAGE_EN,
            "fr",    CFG_LANGUAGE_FR,
            "de",    CFG_LANGUAGE_DE,
            "it",    CFG_LANGUAGE_IT,
            "es",    CFG_LANGUAGE_ES,
            "zh_CN", CFG_LANGUAGE_ZH,
            "ko",    CFG_LANGUAGE_KO,
            "nl",    CFG_LANGUAGE_NL,
            "pt",    CFG_LANGUAGE_PT,
            "ru",    CFG_LANGUAGE_RU,
            "zh_TW", CFG_LANGUAGE_TW
        };

        static constexpr BidirectionalMap models = {
            "CTR", CFG_MODEL_3DS,
            "SPR", CFG_MODEL_3DSXL,
            "KTR", CFG_MODEL_N3DS,
            "FTR", CFG_MODEL_2DS,
            "RED", CFG_MODEL_N3DSXL,
            "JAN", CFG_MODEL_N2DSXL
        };

        static constexpr BidirectionalMap countryCodes = {
            "JP", CFG_REGION_JPN,
            "US", CFG_REGION_USA,
            "EU", CFG_REGION_EUR,
            "AU", CFG_REGION_AUS,
            "CN", CFG_REGION_CHN,
            "KR", CFG_REGION_KOR,
            "TW", CFG_REGION_TWN
        };
        // clang-format on
    };
} // namespace love
