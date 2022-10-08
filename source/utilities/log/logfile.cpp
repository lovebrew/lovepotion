#include <common/console.hpp>
#include <utilities/threads/threads.hpp>

#include <filesystem>
#include <stdarg.h>
#include <stdio.h>

#include <utilities/log/logfile.h>

using namespace love;

static FILE* file = nullptr;
static love::mutex fileMutex;
static bool opened = false;

constexpr const char* LOG_FORMAT = "%s(%lu:%lu): `%s`:\n%s\n\n";

static void init(const char* filepath)
{
    if (opened)
        return;

    file = fopen(filepath, "w");

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

    // clang-format off
    fprintf(file, LOG_FORMAT, filepath.filename().c_str(), location.line(), location.column(), location.function_name(), buffer);
    fflush(file);
    // clang-format on
}
