#pragma once

#include <source_location>

#include <memory>
#include <string>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include <common/console.hpp>

class Log
{
  public:
    static Log& Instance(bool enable = false)
    {
        static Log instance(enable);
        return instance;
    }

    ~Log()
    {
        fclose(this->file);
    }

    template<typename... FormatArgs>
    void Write(std::source_location location, const char* format, FormatArgs&&... args)
    {
        if (!m_enabled)
            return;

        const auto size                = snprintf(nullptr, 0, format, args...);
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size + 1);

        snprintf(buffer.get(), size + 1, format, args...);

        const auto path     = std::string(location.file_name());
        const auto filename = path.substr(path.find_last_of('/') + 1);

        const auto line     = (uint32_t)location.line();
        const auto column   = (uint32_t)location.column();
        const auto funcname = location.function_name();

        fprintf(this->file, BUFFER_FORMAT, filename.c_str(), line, column, funcname, buffer.get());
        fflush(this->file);
    }

  private:
    static inline const char* FILENAME         = "debug.log";
    static constexpr const char* BUFFER_FORMAT = "%s(%zu:%zu): `%s`:\n%s\n\n";

    Log(bool enable) : file(nullptr)
    {
        if (m_enabled || enable)
            this->file = fopen(FILENAME, "w");
    }

    FILE* file;
    static constexpr bool m_enabled = (__DEBUG__);
};

#if __DEBUG__ == 0
    #define LOG(...)
#else
    #define LOG(format, ...) \
        Log::Instance().Write(std::source_location::current(), format, ##__VA_ARGS__);
#endif
