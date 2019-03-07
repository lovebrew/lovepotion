#include "common/runtime.h"

#include <switch.h>
#include <vector>
#include <map>

std::vector<std::string> KEYS =
{
    "a", "b", "x", "y",
    "leftstick", "rightstick",
    "l", "r", "zl", "zr",
    "plus", "minus", "dpleft",
    "dpup", "dpright", "dpdown",
    "", "", "", "", "", "", "", "",
    "sl", "sr", "sl", "sr"
};

std::vector<HidControllerID> CONTROLLER_IDS =
{
    CONTROLLER_PLAYER_1, CONTROLLER_PLAYER_2,
    CONTROLLER_PLAYER_3, CONTROLLER_PLAYER_4,
    CONTROLLER_PLAYER_5, CONTROLLER_PLAYER_6,
    CONTROLLER_PLAYER_7, CONTROLLER_PLAYER_8
};

std::vector<std::string> GAMEPAD_AXES =
{
    "leftx", "lefty",
    "rightx", "righty"
};

std::map<int, std::string> REGIONS =
{
    {SetRegion_JPN, "JPN"},
    {SetRegion_USA, "USA"},
    {SetRegion_EUR, "EUR"},
    {SetRegion_AUS, "AUS"}
};

std::map<int, std::string> LANGUAGES =
{
    {SetLanguage_JA,    "Japanese"               },
    {SetLanguage_ENUS,  "American English"       },
    {SetLanguage_FR,    "French"                 },
    {SetLanguage_DE,    "German"                 },
    {SetLanguage_IT,    "Italian"                },
    {SetLanguage_ES,    "Spanish"                },
    {SetLanguage_ZHCN,  "Chinese"                },
    {SetLanguage_KO,    "Korean"                 },
    {SetLanguage_NL,    "Dutch"                  },
    {SetLanguage_PT,    "Portuguese"             },
    {SetLanguage_RU,    "Russian"                },
    {SetLanguage_ZHTW,  "Taiwanese"              },
    {SetLanguage_ENGB,  "British English"        },
    {SetLanguage_FRCA,  "Canadian French"        },
    {SetLanguage_ES419, "Latin American Spanish" }
};