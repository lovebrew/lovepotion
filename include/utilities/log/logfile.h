#pragma once

#include <common/console.hpp>
#include <source_location>

void logFormat(std::source_location location, const char* format, ...);

#define LOG(format, ...) logFormat(std::source_location::current(), format, ##__VA_ARGS__)
