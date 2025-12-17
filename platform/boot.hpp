#pragma once

#include "common/luax.hpp"

#include <memory>
#include <string>

#define E_FAILED_TO_INIT "Failed to init {:s}: {:#X}\n"

namespace love
{
    namespace platform
    {
        int initialize();

        void errorHandler(const std::string& message);

        std::string getApplicationPath(const std::string& argv0);

        bool run(lua_State* L, int argc, int* nres);

        static constexpr const char* DEFAULT_ARGV0 = "embedded boot.lua";
        inline bool isDefaultEnvironment(const std::string& argv0)
        {
            return (argv0 == DEFAULT_ARGV0 || argv0.empty());
        }

        void shutdown();
    } // namespace platform
} // namespace love
