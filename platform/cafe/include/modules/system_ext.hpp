#pragma once

#include <modules/system/system.tcc>

#include <coreinit/mcp.h>

extern "C"
{
    // remove all this if wut adds it and it causes errors
    typedef struct
    {
        uint32_t major;
        uint32_t minor;
        uint32_t patch;
        char region;
    } MCP_SystemVersion;
    static_assert(sizeof(MCP_SystemVersion) == 0x10);

    int MCP_GetSystemVersion(int handle, MCP_SystemVersion* version);
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

        static bool GetConstant(const char* in, SystemModel& out);
        static bool GetConstant(SystemModel in, const char*& out);

      private:
        int32_t handle;
        uint8_t slot;
    };
} // namespace love
