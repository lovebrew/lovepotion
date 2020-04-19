#pragma once

#if defined (_3DS)
    #define LANGUAGE_COUNT 12
#elif defined (__SWITCH__)
    #define LANGUAGE_COUNT 17
#endif

namespace Horizon
{
    struct PowerInfo
    {
        std::string state;
        int percentage;
    };

    struct NetworkInfo
    {
        std::string status;
        int signal;
    };

    inline int CPU_COUNT = 0;
    inline std::string USERNAME = "";
    inline std::string LANGUAGE = "";

    extern std::array<std::string, LANGUAGE_COUNT> LANGUAGES;

    int GetCPUCount();

    std::string GetUsername();

    std::string GetSystemLanguage();

    PowerInfo GetPowerInfo();

    NetworkInfo GetNetworkInfo();
}
