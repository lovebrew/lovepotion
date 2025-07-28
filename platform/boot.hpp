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

        void shutdown();
    } // namespace platform
} // namespace love
