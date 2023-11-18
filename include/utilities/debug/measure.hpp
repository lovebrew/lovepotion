#pragma once

#include <modules/timer_ext.hpp>
#include <utilities/debug/logfile.hpp>

namespace timer
{
    template<typename T>
    void Measure(const std::string& name, const T& func)
    {
        const double start = love::Timer<Console::Which>::GetTime();
        func();
        const double end = love::Timer<Console::Which>::GetTime();

        LOG("Time taken by %s: %fms", name.c_str(), end - start);
    }
} // namespace timer
