#pragma once

#include "modules/system/System.tcc"

#include <3ds.h>

namespace love
{
    class System : public SystemBase<System>
    {
      public:
        int getProcessorCount() const;

        PowerState getPowerInfo(int& seconds, int& percent) const;

        NetworkState getNetworkInfo(uint8_t& signal) const;

        FriendInfo getFriendInfo() const;

        SystemInfo getOSInfo() const;

        int getPlayCoins() const;

        void setPlayCoins(int amount);

        std::vector<std::string> getPreferredLocales() const;

        // clang-format off
        STRINGMAP_DECLARE(systemLanguages, CFG_Language,
            { "jp",    CFG_LANGUAGE_JP },
            { "en",    CFG_LANGUAGE_EN },
            { "fr",    CFG_LANGUAGE_FR },
            { "de",    CFG_LANGUAGE_DE },
            { "it",    CFG_LANGUAGE_IT },
            { "es",    CFG_LANGUAGE_ES },
            { "zh_CN", CFG_LANGUAGE_ZH },
            { "ko",    CFG_LANGUAGE_KO },
            { "nl",    CFG_LANGUAGE_NL },
            { "pt",    CFG_LANGUAGE_PT },
            { "ru",    CFG_LANGUAGE_RU },
            { "zh_TW", CFG_LANGUAGE_TW }
        );

        STRINGMAP_DECLARE(systemRegions, CFG_Region,
            { "JPN", CFG_REGION_JPN },
            { "USA", CFG_REGION_USA },
            { "EUR", CFG_REGION_EUR },
            { "AUS", CFG_REGION_AUS },
            { "CHN", CFG_REGION_CHN },
            { "KOR", CFG_REGION_KOR },
            { "TWN", CFG_REGION_TWN }
        );

        STRINGMAP_DECLARE(systemModels, CFG_SystemModel,
            { "CTR",  CFG_MODEL_3DS    },
            { "SPR",  CFG_MODEL_3DSXL  },
            { "KTR",  CFG_MODEL_N3DS   },
            { "FTR",  CFG_MODEL_2DS    },
            { "RED",  CFG_MODEL_N3DSXL },
            { "JAN",  CFG_MODEL_N2DSXL }
        );

        using SystemBase::getConstant;
        // clang-format on
    };
} // namespace love
