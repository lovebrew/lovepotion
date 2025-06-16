#pragma once

#include "modules/system/System.tcc"

#include <coreinit/bsp.h>
#include <coreinit/mcp.h>
#include <coreinit/userconfig.h>

#include <coreinit/core.h>
#include <coreinit/mcp.h>

#include <nn/ac/ac_c.h>
#include <nn/act/client_cpp.h>

extern "C"
{
    enum USCLanguage
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
        USCLanguage_TW = 11
    };
}

namespace love
{
    class System : public SystemBase
    {
      public:
        System();

        virtual ~System();

        int getProcessorCount() const;

        PowerState getPowerInfo(int& seconds, int& percent) const;

        std::vector<std::string> getPreferredLocales() const;

        NetworkState getNetworkInfo(uint8_t& signal) const;

        FriendInfo getFriendInfo() const;

        ProductInfo getProductInfo() const;

        using SystemBase::getConstant;

        // clang-format off
        STRINGMAP_DECLARE(SystemModels, BSPHardwareVersion,
            { "lattea11ev",   BSP_HARDWARE_VERSION_LATTE_A11_EV   } ,
            { "lattea11cat",  BSP_HARDWARE_VERSION_LATTE_A11_CAT  } ,
            { "lattea12ev",   BSP_HARDWARE_VERSION_LATTE_A12_EV   } ,
            { "lattea12cat",  BSP_HARDWARE_VERSION_LATTE_A12_CAT  } ,
            { "lattea2xev",   BSP_HARDWARE_VERSION_LATTE_A2X_EV   } ,
            { "lattea2xcat",  BSP_HARDWARE_VERSION_LATTE_A2X_CAT  } ,
            { "lattea3xev",   BSP_HARDWARE_VERSION_LATTE_A3X_EV   } ,
            { "lattea3xcat",  BSP_HARDWARE_VERSION_LATTE_A3X_CAT  } ,
            { "lattea3xcafe", BSP_HARDWARE_VERSION_LATTE_A3X_CAFE } ,
            { "lattea4xev",   BSP_HARDWARE_VERSION_LATTE_A4X_EV   } ,
            { "lattea4xcat",  BSP_HARDWARE_VERSION_LATTE_A4X_CAT  } ,
            { "lattea4xcafe", BSP_HARDWARE_VERSION_LATTE_A4X_CAFE } ,
            { "lattea5xev",   BSP_HARDWARE_VERSION_LATTE_A5X_EV   } ,
            { "lattea5xevy",  BSP_HARDWARE_VERSION_LATTE_A5X_EV_Y } ,
            { "lattea5xcat",  BSP_HARDWARE_VERSION_LATTE_A5X_CAT  } ,
            { "lattea5xcafe", BSP_HARDWARE_VERSION_LATTE_A5X_CAFE } ,
            { "latteb1xev",   BSP_HARDWARE_VERSION_LATTE_B1X_EV   } ,
            { "latteb1xevy",  BSP_HARDWARE_VERSION_LATTE_B1X_EV_Y } ,
            { "latteb1xcat",  BSP_HARDWARE_VERSION_LATTE_B1X_CAT  } ,
            { "latteb1xcafe", BSP_HARDWARE_VERSION_LATTE_B1X_CAFE }
        );

        STRINGMAP_DECLARE(SystemLanguages, USCLanguage,
            { "jp",    USCLanguage_JA },
            { "en",    USCLanguage_EN },
            { "fr",    USCLanguage_FR },
            { "de",    USCLanguage_DE },
            { "it",    USCLanguage_IT },
            { "es",    USCLanguage_ES },
            { "zh_CN", USCLanguage_ZH },
            { "ko",    USCLanguage_KO },
            { "nl",    USCLanguage_NL },
            { "pt",    USCLanguage_PT },
            { "ru",    USCLanguage_RU },
            { "zh_TW", USCLanguage_TW }
        );

        STRINGMAP_DECLARE(SystemRegions, MCPRegion,
            { "jp", MCP_REGION_JAPAN  },
            { "us", MCP_REGION_USA    },
            { "eu", MCP_REGION_EUROPE },
            { "cn", MCP_REGION_CHINA  },
            { "kr", MCP_REGION_KOREA  },
            { "tw", MCP_REGION_TAIWAN }
        );
        // clang-format on

      private:
        int32_t mcpHandle;
        UCHandle ucHandle;
        uint8_t account;
    };
} // namespace love
