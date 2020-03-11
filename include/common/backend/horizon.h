/*
** horizon.h
** Handles internal username/WiFi information
*/

#pragma once

class Horizon
{
    public:
        static void Initialize();

        static std::string GetUsername();

        int GetProcessorCount();

        static std::pair<std::string, u8> GetPowerInfo();

        static std::string GetRegion();

        static std::string GetLanguage();

        static std::pair<std::string, int> GetWiFiStatus();

        static void Exit();

    private:
        static std::map<int, std::string> m_regions;

        static std::map<int, std::string> m_languages;

        static inline bool initialized = false;
};
