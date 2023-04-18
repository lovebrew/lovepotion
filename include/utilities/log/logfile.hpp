#pragma once

#include <source_location>
#include <string>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include <common/console.hpp>

class Log
{
  public:
    static Log& Instance()
    {
        static Log instance;
        return instance;
    }

    ~Log()
    {
        fclose(this->file);
    }

    void Write(std::source_location location, const char* format, ...)
    {
        if (!m_enabled)
            return;

        va_list args;
        char buffer[BUFFER_LIMIT] { '\0' };

        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        const auto path     = std::string(location.file_name());
        const auto filename = path.substr(path.find_last_of('/') + 1);

        const auto line     = (uint32_t)location.line();
        const auto column   = (uint32_t)location.column();
        const auto funcname = location.function_name();

#if defined(__3DS__)
        fprintf(this->file, BUFFER_FORMAT, filename.c_str(), line, column, funcname, buffer);
#else
        fprintf(this->file, BUFFER_FORMAT2, filename.c_str(), line, column, funcname, buffer);
#endif

        fflush(this->file);
    }

  private:
    static inline const char* FILENAME = "debug.log";

    static constexpr const char* BUFFER_FORMAT  = "%s(%lu:%lu): `%s`:\n%s\n\n";
    static constexpr const char* BUFFER_FORMAT2 = "%s(%u:%u): `%s`:\n%s\n\n";

    static constexpr size_t BUFFER_LIMIT = 0x200;

    Log() : file(nullptr)
    {
        if (m_enabled)
            this->file = fopen(FILENAME, "w");
    }

    FILE* file;
    static constexpr bool m_enabled = (__DEBUG__);
};

#if __DEBUG__ == 0
    #define LOG(...)
#else
    #define LOG(format, ...) \
        Log::Instance().Write(std::source_location::current(), format, ##__VA_ARGS__)
#endif
