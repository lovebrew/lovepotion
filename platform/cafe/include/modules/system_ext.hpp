#pragma once

#include <modules/system/system.tcc>

#include <coreinit/bsp.h>
#include <coreinit/mcp.h>
#include <coreinit/userconfig.h>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

extern "C"
{
    typedef enum USCLanguage : int8_t
    {
        USCLanguage_JA = 0,
        USCLanguage_EN = 1,
        USCLanguage_FR = 2,
        USCLanguage_DE = 3,
        USCLanguage_IT = 4,
        USCLanguage_ES = 5,
        USCLanguage_ZH = 6,
        USCLanguage_KO = 7,
        USCLanguage_NL = 8,
        USCLanguage_PT = 9,
        USCLanguage_RU = 10,
        USCLanguage_TW = 11,
    };
}

namespace love
{
    template<>
    class System<Console::CAFE> : public System<Console::ALL>
    {
      public:
        enum SystemModel
        {
            SYSTEM_MODEL_BASIC,
            SYSTEM_MODEL_DELUXE,
            SYSTEM_MODEL_MAX_ENUM
        };

        System();

        ~System();

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
        static constexpr BidirectionalMap systemModels = {
            "Hollywood English Sample 1",   BSP_HARDWARE_VERSION_HOLLYWOOD_ENG_SAMPLE_1,
            "Hollywood English Sample 2",   BSP_HARDWARE_VERSION_HOLLYWOOD_ENG_SAMPLE_2,
            "Hollywood Production For Wii", BSP_HARDWARE_VERSION_HOLLYWOOD_PROD_FOR_WII,
            "Hollywood Cortado",            BSP_HARDWARE_VERSION_HOLLYWOOD_CORTADO,
            "Hollywood Cortado Espresso",   BSP_HARDWARE_VERSION_HOLLYWOOD_CORTADO_ESPRESSO,
            "Bollywood",                    BSP_HARDWARE_VERSION_BOLLYWOOD,
            "Bollywood Production For Wii", BSP_HARDWARE_VERSION_BOLLYWOOD_PROD_FOR_WII,
            "Latte A11 Ev",                 BSP_HARDWARE_VERSION_LATTE_A11_EV,
            "Latte A11 Cat",                BSP_HARDWARE_VERSION_LATTE_A11_CAT,
            "Latte A12 Ev",                 BSP_HARDWARE_VERSION_LATTE_A12_EV,
            "Latte A12 Cat",                BSP_HARDWARE_VERSION_LATTE_A12_CAT,
            "Latte A2x Ev",                 BSP_HARDWARE_VERSION_LATTE_A2X_EV,
            "Latte A2x Cat",                BSP_HARDWARE_VERSION_LATTE_A2X_CAT,
            "Latte A3x Ev",                 BSP_HARDWARE_VERSION_LATTE_A3X_EV,
            "Latte A3x Cat",                BSP_HARDWARE_VERSION_LATTE_A3X_CAT,
            "Latte A3x Cafe",               BSP_HARDWARE_VERSION_LATTE_A3X_CAFE,
            "Latte A4x Ev",                 BSP_HARDWARE_VERSION_LATTE_A4X_EV,
            "Latte A4x Cat",                BSP_HARDWARE_VERSION_LATTE_A4X_CAT,
            "Latte A4x Cafe",               BSP_HARDWARE_VERSION_LATTE_A4X_CAFE,
            "Latte A5x Ev",                 BSP_HARDWARE_VERSION_LATTE_A5X_EV,
            "Latte A5x Ev Y",               BSP_HARDWARE_VERSION_LATTE_A5X_EV_Y,
            "Latte A5x Cat",                BSP_HARDWARE_VERSION_LATTE_A5X_CAT,
            "Latte A5x Cafe",               BSP_HARDWARE_VERSION_LATTE_A5X_CAFE,
            "Latte B1x Ev",                 BSP_HARDWARE_VERSION_LATTE_B1X_EV,
            "Latte B1x Ev Y",               BSP_HARDWARE_VERSION_LATTE_B1X_EV_Y,
            "Latte B1x Cat",                BSP_HARDWARE_VERSION_LATTE_B1X_CAT,
            "Latte B1x Cafe",               BSP_HARDWARE_VERSION_LATTE_B1X_CAFE
        };

        static constexpr BidirectionalMap languages = {
            "jp",    USCLanguage_JA,
            "en",    USCLanguage_EN,
            "fr",    USCLanguage_FR,
            "de",    USCLanguage_DE,
            "it",    USCLanguage_IT,
            "es",    USCLanguage_ES,
            "zh_CN", USCLanguage_ZH,
            "ko",    USCLanguage_KO,
            "nl",    USCLanguage_NL,
            "pt",    USCLanguage_PT,
            "ru",    USCLanguage_RU,
            "zh_TW", USCLanguage_TW
        };

        static constexpr BidirectionalMap countryCodes = {
            "JP", MCP_REGION_JAPAN,
            "US", MCP_REGION_USA,
            "EU", MCP_REGION_EUROPE,
            "CN", MCP_REGION_CHINA,
            "KR", MCP_REGION_KOREA,
            "TW", MCP_REGION_TAIWAN
        };

        // clang-format on

      private:
        struct
        {
            int32_t mcp;
            UCHandle userConfig;
        } handles;

        uint8_t accountSlot;
    };
} // namespace love
