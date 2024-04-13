#pragma once

#include <source_location>

#include <format>
#include <string>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

class Log
{
  public:
    static Log& getInstance()
    {
        static Log instance;
        return instance;
    }

    ~Log()
    {}

    template<typename... Args>
    void write(std::source_location location, std::format_string<Args...> format, Args&&... args)
    {
        auto output = std::format(format, std::forward<Args>(args)...);

        const auto path     = std::string_view(location.file_name());
        const auto filename = path.substr(path.find_last_of('/') + 1);

        const auto line   = (size_t)location.line();
        const auto column = (size_t)location.column();

        auto buffer = std::format(BUFFER_FORMAT, filename, line, column, output);

        std::fwrite(buffer.c_str(), 1, buffer.size(), stdout);
        std::fflush(stdout);
    }

  private:
    static constexpr const char* BUFFER_FORMAT = "{:s}({:d}:{:d}): {:s}\n";
};

#if __DEBUG__ == 0
    #define LOG(...)
#else
    #define LOG(format, ...) \
        Log::getInstance().write(std::source_location::current(), format, ##__VA_ARGS__);
#endif
