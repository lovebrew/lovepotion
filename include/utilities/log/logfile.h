#pragma once

#include <source_location>

void logFormat(std::source_location location, const char* format, ...);

#define WIIU_USE_OSREPORT 1

#if !(WIIU_USE_OSREPORT)
    #define LOG(format, ...) logFormat(std::source_location::current(), format, ##__VA_ARGS__)
#else
    #if defined(__WIIU__)
        #include <coreinit/debug.h>
        #define LOG(format, ...) OSReport(format, ##__VA_ARGS__)
    #endif
#endif
