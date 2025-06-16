#pragma once

#include "modules/system/System.tcc"

#include <switch.h>

namespace love
{
    class System : public SystemBase
    {
      public:
        System();

        int getProcessorCount() const;

        PowerState getPowerInfo(int& seconds, int& percent) const;

        NetworkState getNetworkInfo(uint8_t& signal) const;

        FriendInfo getFriendInfo() const;

        ProductInfo getProductInfo() const;

        std::vector<std::string> getPreferredLocales() const;

        // clang-format off
        STRINGMAP_DECLARE(SystemLanguages, SetLanguage,
            { "jp",      SetLanguage_JA     },
            { "en_US",   SetLanguage_ENUS   },
            { "fr",      SetLanguage_FR     },
            { "de",      SetLanguage_DE     },
            { "it",      SetLanguage_IT     },
            { "es",      SetLanguage_ES     },
            { "zh_CN",   SetLanguage_ZHCN   },
            { "ko",      SetLanguage_KO     },
            { "nl",      SetLanguage_NL     },
            { "pt",      SetLanguage_PT     },
            { "ru",      SetLanguage_RU     },
            { "zh_TW",   SetLanguage_ZHTW   },
            { "en_GB",   SetLanguage_ENGB   },
            { "fr_CA",   SetLanguage_FRCA   },
            { "es_419",  SetLanguage_ES419  },
            { "zh_HANS", SetLanguage_ZHHANS },
            { "zh_HANT", SetLanguage_ZHHANT },
            { "pt_BR",   SetLanguage_PTBR   }
        );

        STRINGMAP_DECLARE(SystemModels, SetSysProductModel,
            { "invalid", SetSysProductModel_Invalid },
            { "nx",      SetSysProductModel_Nx      },
            { "copper",  SetSysProductModel_Copper  },
            { "iowa",    SetSysProductModel_Iowa    },
            { "hoag",    SetSysProductModel_Hoag    },
            { "calico",  SetSysProductModel_Calcio  },
            { "aula",    SetSysProductModel_Aula    }
        );

        STRINGMAP_DECLARE(SystemRegions, SetRegion,
            { "jpn", SetRegion_JPN },
            { "usa", SetRegion_USA },
            { "eur", SetRegion_EUR },
            { "aus", SetRegion_AUS },
            { "htk", SetRegion_HTK },
            { "chn", SetRegion_CHN }
        );

        STRINGMAP_DECLARE(SystemColorThemes, ColorSetId,
            { "light", ColorSetId_Light },
            { "dark",  ColorSetId_Dark  }
        );

        using SystemBase::getConstant;

      private:
        AccountUid account;
    };
} // namespace love
