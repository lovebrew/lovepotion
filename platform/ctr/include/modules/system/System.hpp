#pragma once

#include "modules/system/System.tcc"

#include <3ds.h>

namespace love
{
    using SystemLanguage = CFG_Language;
    using SystemRegion   = CFG_Region;
    using SystemModel    = CFG_SystemModel;

    class System : public SystemBase
    {
      public:
        int getProcessorCount() const override;

        PowerState getPowerInfo(int& seconds, int& percent) const override;

        NetworkState getNetworkInfo(int32_t& signal) const override;

        bool getFriendInfo(FriendInfo& info) const override;

        bool getInfo(ProductInfo& info) const override;

        std::vector<std::string> getPreferredLocales() const override;

        int getPlayCoins() const;

        void setPlayCoins(int amount);

        // clang-format off
        STRINGMAP_DECLARE(SystemLanguages, CFG_Language,
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

        STRINGMAP_DECLARE(SystemRegions, CFG_Region,
            { "jpn", CFG_REGION_JPN },
            { "usa", CFG_REGION_USA },
            { "eur", CFG_REGION_EUR },
            { "aus", CFG_REGION_AUS },
            { "chn", CFG_REGION_CHN },
            { "kor", CFG_REGION_KOR },
            { "twn", CFG_REGION_TWN }
        );

        STRINGMAP_DECLARE(SystemModels, CFG_SystemModel,
            { "ctr",  CFG_MODEL_3DS    },
            { "spr",  CFG_MODEL_3DSXL  },
            { "ktr",  CFG_MODEL_N3DS   },
            { "ftr",  CFG_MODEL_2DS    },
            { "red",  CFG_MODEL_N3DSXL },
            { "jan",  CFG_MODEL_N2DSXL }
        );

        using SystemBase::getConstant;
        // clang-format on
    };
} // namespace love
