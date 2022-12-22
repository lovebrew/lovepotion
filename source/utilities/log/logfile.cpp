#include <common/console.hpp>
#include <utilities/threads/threads.hpp>

#include <filesystem>
#include <stdarg.h>
#include <stdio.h>

#include <utilities/log/logfile.h>

#if defined(__WIIU__)
    #include <coreinit/debug.h>
#endif

using namespace love;

static FILE* file = nullptr;
static love::mutex fileMutex;
static bool opened = false;

constexpr const char* LOG_FORMAT = "%s(%u:%u): `%s`:\n%s\n\n";

static void init(const char* filepath)
{
    if (opened)
        return;

    file = fopen(filepath, "a");

    if (!file)
        fclose(file);
    else
        opened = true;
}

static constexpr const char* getFilepath()
{
    return (Console::Is(Console::CAFE)) ? "wiiu/apps/lovepotion/love.log" : "love.log";
}

void logFormat(std::source_location location, const char* format, ...)
{
    std::unique_lock lock(fileMutex);

    init(getFilepath());

    char buffer[256] { '\0' };

    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    std::filesystem::path filepath(location.file_name());

    const char* filename = filepath.filename().c_str();
    const auto line      = (uint32_t)location.line();
    const auto column    = (uint32_t)location.column();
    const char* funcname = location.function_name();

    fprintf(file, LOG_FORMAT, filename, line, column, funcname, buffer);
    fflush(file);
}
