#include "common/runtime_common.h"

#include <3ds.h>
#include <vector>
#include <map>

std::vector<string> KEYS =
{
    "a", "b", "select", "start",
    "dpright", "dpleft", "dpup", "dpdown",
    "r", "l", "x", "y",
    "", "", "zl", "zr",
    "", "", "", "",
    "touch", "", "", "", "cstickright",
    "cstickleft", "cstickup", "cstickdown",
    "cpadright", "cpadleft", "cpadup", "cpaddown"
};

std::vector<std::string> GAMEPAD_AXES = 
{
    "leftx", "lefty", "rightx", "righty"
};

std::map<int, std::string> REGIONS =
{
    { CFG_REGION_AUS, "AUS" },
    { CFG_REGION_CHN, "CHN" },
    { CFG_REGION_EUR, "EUR" },
    { CFG_REGION_JPN, "JPN" },
    { CFG_REGION_KOR, "KOR" },
    { CFG_REGION_TWN, "TWN" },
    { CFG_REGION_USA, "USA" }
};

std::map<int, std::string> LANGUAGES =
{
    { CFG_LANGUAGE_DE, "German"              },
    { CFG_LANGUAGE_EN, "English"             },
    { CFG_LANGUAGE_ES, "Spanish"             },
    { CFG_LANGUAGE_FR, "French"              },
    { CFG_LANGUAGE_IT, "Italian"             },
    { CFG_LANGUAGE_JP, "Japanese"            },
    { CFG_LANGUAGE_KO, "Korean"              },
    { CFG_LANGUAGE_NL, "Dutch"               },
    { CFG_LANGUAGE_PT, "Portugese"           },
    { CFG_LANGUAGE_RU, "Russian"             },
    { CFG_LANGUAGE_TW, "Traditional Chinese" },
    { CFG_LANGUAGE_ZH, "Simplified Chinese"  },
};